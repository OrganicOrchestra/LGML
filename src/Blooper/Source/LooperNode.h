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
	LooperNode(NodeManager * nodeManager,uint32 nodeId) :NodeBase(nodeManager,nodeId,"Looper",new Looper) {}
    
    
    
    
    class Looper : public NodeAudioProcessor{
    public:
        Looper():globalSampleTime(0),maxLoopLength(44100*2){
        
        }
        
        
        void processBlockInternal(AudioBuffer<float>& buffer,
                                  MidiBuffer& midiMessages)override;
        
        
        
        int globalSampleTime;
        int maxLoopLength;
        
        
        void setNumTracks(int numTracks);
        
        class Track{
        public:
            
            
            Track(Looper * looper,int _trackNum):parentLooper(looper),
            trackNum("trackNum","trackNum",0,MAX_NUM_TRACKS,_trackNum)
            
            {
    
            }
            
            
            AudioParameterBool *  shouldRecord;
            AudioParameterBool * shouldPlay;
            AudioParameterBool * shouldClear;
            
            
            AudioParameterFloat * volume;
            float lastVolume;

        private:
            AudioParameterInt   trackNum;
            
            
            
            AudioParameterBool *  isRecording;
            Atomic<int> recordNeedle;
            int recordingDelay;
            int quantizedRecordEnd,quantizedRecordStart;
            
            AudioParameterBool * isPlaying;
            int playNeedle;
            int quantizedPlayStart,quantizedPlayEnd;
            void updatePendingLooperState(int64 curTime);
            
            
            
            
            AudioSampleBuffer monoLoopSample;
            
            // keeps track of few bits of audio
            // to readjust the loop when controllers are delayed
            

            BipBuffer streamBipBuffer;
            AudioParameterInt * streamBipBufferDelay;

            Looper * parentLooper;
            
            void processBlock(AudioBuffer<float>& buffer, MidiBuffer & midi);
            
            friend class Looper;
        };
        
        
        
        
        OwnedArray<Track> tracks;
        // RMS Values from all Tracks
        Array<Array<float> > RMS;

        
    };
    
    

	JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(LooperNode)
};

#endif  // LOOPERNODE_H_INCLUDED
