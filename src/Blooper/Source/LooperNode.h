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
    LooperNode(NodeManager * nodeManager,uint32 nodeId) :NodeBase(nodeManager,nodeId,"Looper",new Looper) {
        looper = dynamic_cast<Looper*>(audioProcessor);
    }
    
    

    
    class Looper : public NodeAudioProcessor{
    public:
        Looper(){
            setNumTracks(8);
        }
        
        
        void processBlockInternal(AudioBuffer<float>& buffer,
                                  MidiBuffer& midiMessages)override;
        
        

        
        
        void setNumTracks(int numTracks);
        
        class Track : Trigger::Listener{
        public:
            
            
            Track(Looper * looper,int _trackNum):
            parentLooper(looper),
            trackNum("trackNum",_trackNum,0,MAX_NUM_TRACKS),
            shouldRecordTrig("shouldRecord"),quantizedRecordStart(0),quantizedRecordEnd(0),
            shouldPlayTrig("shouldPlay"),quantizedPlayStart(0),quantizedPlayEnd(0),
            shouldClearTrig("shouldClear"),
            volume("volume",.85,0,1),
            preDelayMs("preDelayMs",0,0,200),
            streamBipBuffer(16384),// 16000 ~ 300ms and 256*64
            monoLoopSample(1,44100*MAX_LOOP_LENGTH_S),
            trackState(STOPPED)
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

        
        OwnedArray<Track> tracks;

        AudioBuffer<float> bufferIn;

        AudioBuffer<float>bufferOut;
        
    };
    
    
    Looper * looper;
    NodeBaseUI * createUI() override;

	JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(LooperNode)
};

#endif  // LOOPERNODE_H_INCLUDED
