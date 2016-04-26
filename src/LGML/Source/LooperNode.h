/*
 ==============================================================================

 LooperNode.h
 Created: 3 Mar 2016 10:32:16pm
 Author:  bkupe

 ==============================================================================
 */

#ifndef LOOPERNODE_H_INCLUDED
#define LOOPERNODE_H_INCLUDED


#include "NodeBase.h"
#include "AudioHelpers.h"


#define MAX_LOOP_LENGTH_S 30
#define MAX_NUM_TRACKS 32



class LooperNode : public NodeBase
{

public:
    LooperNode(NodeManager * nodeManager,uint32 nodeId);



    class Looper : public NodeAudioProcessor, public ControllableContainer
    {

    public:
        Trigger * recPlaySelectedTrig;
        Trigger * playSelectedTrig;
        Trigger * clearSelectedTrig;
        Trigger * stopSelectedTrig;

        Trigger * clearAllTrig;
        Trigger * stopAllTrig;


        FloatParameter * volumeSelected;
        BoolParameter * isMonitoring;
        IntParameter * numberOfTracks;



        class Track;
        OwnedArray<Track> tracks;

        Looper(LooperNode * looperNode);



        void setNumTracks(int numTracks);
        void addTrack();
        void removeTrack( int i);



        class Track : public ControllableContainer
        {
        public:
            Track(Looper * looper, int _trackIdx);

            ~Track(){}

			Trigger * selectTrig;
			Trigger * recPlayTrig;
            Trigger * playTrig;
            Trigger * clearTrig;
            Trigger * stopTrig;
            StringParameter  * stateParameterString;
            FloatParameter * volume;


            enum TrackState{
                SHOULD_RECORD = 0,
                RECORDING,
                SHOULD_PLAY,
                PLAYING,
                SHOULD_CLEAR,
                CLEARED,
                STOPPED
            };
            TrackState trackState;
            static String trackStateToString(const TrackState & ts);
            Component * createControllableContainerEditor()override;

            void setTrackState(TrackState state);
            // from events like UI
            void askForSelection(bool isSelected);
            bool askForBeingMasterTempoTrack();
            Track * getMasterTempoTrack();

            //Listener
            class  Listener : public AsyncUpdater
            {
            public:

                /** Destructor. */
                virtual ~Listener() {}
                //                called from here
                void internalTrackStateChanged( const TrackState &state){
                    stateToBeNotified = state;
                    trackStateChanged(state);
                    triggerAsyncUpdate();
                }
                TrackState stateToBeNotified;

                // dispatched to listeners
                virtual void trackStateChanged(const TrackState &) {};
                virtual void trackStateChangedAsync(const TrackState & state) = 0;
                void handleAsyncUpdate() override{trackStateChangedAsync(stateToBeNotified);}
                virtual void trackSelected(bool){};
            };
            ListenerList<Listener> trackStateListeners;
            void addTrackListener(Listener* newListener) { trackStateListeners.add(newListener); }
            void removeTrackListener(Listener* listener) { trackStateListeners.remove(listener); }




            class AsyncTrackStateStringSynchroizer : public Track::Listener{
            public:
                StringParameter * stringParameter;
                AsyncTrackStateStringSynchroizer(StringParameter  *origin):stringParameter(origin){}
                void trackStateChangedAsync(const TrackState &_trackState)override{
                    stringParameter->setValue(trackStateToString(_trackState));
                }
            };
           ScopedPointer<AsyncTrackStateStringSynchroizer> stateParameterStringSynchronizer;




        private:

            void triggerTriggered(Trigger * t) override;
            void setSelected(bool isSelected);
            int trackIdx;

            int recordNeedle;
            int quantizedRecordEnd,quantizedRecordStart;

            int playNeedle;
            int quantizedPlayStart,quantizedPlayEnd;

            void updatePendingLooperTrackState(const uint64 curTime,int blockSize);


            AudioSampleBuffer monoLoopSample;
            float lastVolume;
            // represent audioProcessor behaviour
            enum InternalTrackState{
                BUFFER_STOPPED = 0,
                BUFFER_PLAYING,
                BUFFER_RECORDING

            };
            InternalTrackState internalTrackState;
            InternalTrackState lastInternalTrackState;

            // keeps track of few bits of audio
            // to readjust the loop when controllers are delayed
            RingBuffer streamAudioBuffer;
            IntParameter * preDelayMs;

            Looper * parentLooper;

            void processBlock(AudioBuffer<float>& buffer, MidiBuffer & midi);
            const float defaultVolumeValue = 0.8f;


            void cleanAllQuantizeNeedles();
            friend class Looper;
        };


        //Listener
        class  Listener
        {
        public:

            /** Destructor. */
            virtual ~Listener() {}
            virtual void trackNumChanged(int num) = 0;
        };


        ListenerList<Listener> looperListeners;
        void addLooperListener(Listener* newListener) { looperListeners.add(newListener); }
        void removeLooperListener(Listener* listener) { looperListeners.remove(listener); }

        bool askForBeingMasterTrack(Track * t){bool res =  areAllTrackClearedButThis(t);
            if(res)lastMasterTempoTrack = t;
            return res;
        }
        bool askForBeingAbleToPlayNow(Track *_t){
            bool result = true;
            for(auto & t:tracks){
                if(t!=_t)result &= (t->trackState == Track::STOPPED)||(t->trackState == Track::CLEARED);
            }
            return result;
        }

        bool areAllTrackClearedButThis(Track * _t){
            bool result = true;
            for(auto & t:tracks){
                if(t!=_t)result &= t->trackState == Track::CLEARED;
            }
            return result;
        }



    private:

        void onContainerTriggerTriggered(Trigger * t) override;
        void onContainerParameterChanged(Parameter * p) override;
        // internal
        void processBlockInternal(AudioBuffer<float>& buffer,MidiBuffer& midiMessages)override;
        void checkIfNeedGlobalLooperStateUpdate();
        void selectMe(Track * t);
        Track * lastMasterTempoTrack;
        Track * selectedTrack;
        AudioBuffer<float> bufferIn;
        AudioBuffer<float>bufferOut;

        LooperNode * looperNode;

    };


    Looper * looper;
    NodeBaseUI * createUI() override;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(LooperNode)
};

#endif  // LOOPERNODE_H_INCLUDED
