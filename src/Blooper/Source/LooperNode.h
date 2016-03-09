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
#define MAX_NUM_TRACKS 30

class LooperNode : public NodeBase
{

public:
    LooperNode(NodeManager * nodeManager,uint32 nodeId) :NodeBase(nodeManager,nodeId,"Looper",new Looper(this)) {
        looper = dynamic_cast<Looper*>(audioProcessor);
    }
    
    

    
    class Looper : public NodeAudioProcessor{
    public:
        Looper(LooperNode * looperNode):looperNode(looperNode){
            setNumTracks(8);
        }
        
        
        void processBlockInternal(AudioBuffer<float>& buffer,
                                  MidiBuffer& midiMessages)override;
        



        
        void setNumTracks(int numTracks);
        
        class Track : Trigger::Listener{
        public:
            Track(Looper * looper,int _trackNum):
            parentLooper(looper),
            trackNum("Track Number","Index of the track",_trackNum,0,MAX_NUM_TRACKS),
            shouldRecordTrig("Should Record","Tells the track to wait for the next bar and then start recording"),quantizedRecordStart(0),quantizedRecordEnd(0),
            shouldPlayTrig("Should Play","Tells the track to wait for the next bar and then stop recording and start playing"),quantizedPlayStart(0),quantizedPlayEnd(0),
            shouldClearTrig("Should Clear","Tells the track to clear it's content if got any"),
            volume("Volume","Set the volume of the track",1,0,1),
            preDelayMs("Pre Delay MS","Pre process delay (in milliseconds)",0,0,200),
            streamBipBuffer(16384),// 16000 ~ 300ms and 256*64
            monoLoopSample(1,44100*MAX_LOOP_LENGTH_S),
            trackState(CLEARED)
            {

                shouldRecordTrig.addListener(this);
                shouldPlayTrig.addListener(this);
                shouldClearTrig.addListener(this);
            }
            
            ~Track(){
                
            }
            
            Trigger  shouldRecordTrig;
            Trigger  shouldPlayTrig;
            Trigger  shouldClearTrig;

            
            
            
            enum TrackState{
                SHOULD_RECORD,
                RECORDING,
                SHOULD_PLAY,
                PLAYING,
                SHOULD_CLEAR,
                CLEARED,
                STOPPED
            };
            TrackState trackState;
            void setTrackState(TrackState state);
            
            
            //Listener
            class  Listener : public AsyncUpdater
            {
            public:
                
                /** Destructor. */
                virtual ~Listener() {}
//                called from here
                void internalTrackStateChanged(const TrackState &state){
                    stateToBeNotified = state;
                    trackStateChanged(state);
                    triggerAsyncUpdate();
                }
                TrackState stateToBeNotified;
                
                // dispatched to listeners
                virtual void trackStateChanged(const TrackState & state) {};
                virtual void trackStateChangedAsync(const TrackState & state) = 0;
                void handleAsyncUpdate() override{
                    trackStateChangedAsync(stateToBeNotified);
                }
            };
            ListenerList<Listener> listeners;
            void addListener(Listener* newListener) { listeners.add(newListener); }
            void removeListener(Listener* listener) { listeners.remove(listener); }
            
            FloatParameter volume;
            float lastVolume;
            
            // RMS Values from all Tracks
            Array<float> RMS;
            bool isMasterTempoTrack();
        private:
            
            void triggerTriggered(Trigger * t)override;
            
            IntParameter   trackNum;
            
            
            

            int recordNeedle;
            int recordingDelay;
            int quantizedRecordEnd,quantizedRecordStart;
            
            int playNeedle;
            int quantizedPlayStart,quantizedPlayEnd;
            void updatePendingLooperTrackState(int64 curTime);
      
            
            AudioSampleBuffer monoLoopSample;
            
            // keeps track of few bits of audio
            // to readjust the loop when controllers are delayed
            BipBuffer streamBipBuffer;
            IntParameter  preDelayMs;

            Looper * parentLooper;
            
            void processBlock(AudioBuffer<float>& buffer, MidiBuffer & midi);
            
            
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
        
        
        ListenerList<Listener> listeners;
        void addListener(Listener* newListener) { listeners.add(newListener); }
        void removeListener(Listener* listener) { listeners.remove(listener); }
        
        bool askForBeingMasterTrack(Track * t){
            if(areAllTrackClearedButThis(t)){
                return true;
            }
            return false;
        }
        bool areAllTrackClearedButThis(Track * _t){
            bool result = true;
            for(auto & t:tracks){
                if(t!=_t)result &= t->trackState == Track::CLEARED;
            }
            return result;
        }
        
        OwnedArray<Track> tracks;

        AudioBuffer<float> bufferIn;
        AudioBuffer<float>bufferOut;
        
        LooperNode * looperNode;
        
    };
    
    
    Looper * looper;
    NodeBaseUI * createUI() override;

	JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(LooperNode)
};

#endif  // LOOPERNODE_H_INCLUDED
