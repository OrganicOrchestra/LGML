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
            shouldRecordTrig("shouldRecord"),shouldRecord(false),isRecording(false),quantizedRecordStart(0),quantizedRecordEnd(0),
            shouldPlayTrig("shouldPlay"),shouldPlay(false),isPlaying(false),quantizedPlayStart(0),quantizedPlayEnd(0),
            shouldClearTrig("shouldClear"),shouldClear(false),
            volume("volume",.85,0,1),
            preDelayMs("preDelayMs",0,0,200),
            streamBipBuffer(16384),// 16000 ~ 300ms and 256*64
            monoLoopSample(1,44100*MAX_LOOP_LENGTH_S)
            {
    
            }
            
            
            
            Trigger  shouldRecordTrig;
            bool shouldRecord;
            Trigger  shouldPlayTrig;
            bool shouldPlay;
            Trigger  shouldClearTrig;
            bool shouldClear;
            
            FloatParameter volume;
            float lastVolume;
            
            // RMS Values from all Tracks
            Array<float> RMS;
        private:
            
            void triggerTriggered(Trigger * t)override;
            
            IntParameter   trackNum;
            
            
            
            bool  isRecording;
            Atomic<int> recordNeedle;
            int recordingDelay;
            int quantizedRecordEnd,quantizedRecordStart;
            
            bool isPlaying;
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
