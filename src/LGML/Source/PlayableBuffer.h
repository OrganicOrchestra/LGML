/*
 ==============================================================================
 
 PlayableBuffer.h
 Created: 6 Jun 2016 7:45:50pm
 Author:  Martin Hermant
 
 ==============================================================================
 */

#ifndef PLAYABLEBUFFER_H_INCLUDED
#define PLAYABLEBUFFER_H_INCLUDED

#include "JuceHeader.h"
#include "DebugHelpers.h"




class PlayableBuffer {
    
    public :
    PlayableBuffer(int numChannels,int numSamples):
    loopSample(numChannels,numSamples),
    recordNeedle(0),
    startJumpNeedle(0),
    playNeedle(0),isJumping(false),
    state(BUFFER_STOPPED),
    lastState(BUFFER_STOPPED),
    stateChanged(false),
    numTimePlayed(0)
    {
        jassert(numSamples < INT_MAX);
        //        for (int j = 0 ; j < numSamples ; j++){int p = 44;float t = (j%p)*1.0/p;float v = t;
        //            for(int i = 0 ; i < numChannels ; i++){loopSample.addSample(i, j, v);}
        //        }
        loopSample.clear();
    }
    
    bool writeAudioBlock(const AudioBuffer<float> & buffer){
        jassert(state==BUFFER_RECORDING);
        if (recordNeedle + buffer.getNumSamples()> loopSample.getNumSamples()) {
            jassertfalse;
            return false;
        }
        else{
            for (int i = loopSample.getNumChannels() - 1; i >= 0; --i) {
                int maxChannel = jmin(i,buffer.getNumChannels()-1);
                loopSample.copyFrom(maxChannel, recordNeedle, buffer, maxChannel, 0, buffer.getNumSamples());
            }
            recordNeedle += buffer.getNumSamples();
        }
        
        return true;
    }
    
    int getNumSamples() const{return loopSample.getNumSamples();}
    
    void readNextBlock(AudioBuffer<float> & buffer
#ifdef BLOCKSIZEGRANULARITY
                       ,bool checkForAlignment
#endif
        ){
        jassert(isOrWasPlaying());


        int numSamples = buffer.getNumSamples();
#ifdef BLOCKSIZEGRANULARITY
      // check if block size is always the same
      if(numSamples>0){
        static int originBlockSize = numSamples;
        jassert(originBlockSize == numSamples);
      }
#endif
        // we want to read Last Block for fade out if stopped
        if(state==BUFFER_STOPPED){
            playNeedle = startJumpNeedle;
        }
        
        // assert false for now to check alignement
        if(isFirstPlayingFrame())jassert(playNeedle==0);
        
        
        // stitch audio jumps by quick fadeIn/Out
        if(isJumping && playNeedle!=startJumpNeedle && state!=BUFFER_STOPPED){
          //LOG("a:jump "<<startJumpNeedle <<","<< playNeedle);

            const int halfBlock =  numSamples/2;
            for (int i = buffer.getNumChannels() - 1; i >= 0; --i) {
                int maxChannelFromRecorded = jmin(loopSample.getNumChannels() - 1, i);
                buffer.copyFrom(i, 0, loopSample, maxChannelFromRecorded, startJumpNeedle, halfBlock);
                buffer.applyGainRamp(i, 0, halfBlock, 1.0f, 0.0f);
                buffer.copyFrom(i, halfBlock, loopSample, maxChannelFromRecorded, playNeedle+halfBlock, halfBlock);
                buffer.applyGainRamp(i, halfBlock-1, halfBlock, 0.0f, 1.0f);
                
            }
        }
        
        else{
            if ((playNeedle + numSamples) > recordNeedle)
            {
                
                
#ifdef BLOCKSIZEGRANULARITY
                //assert false for debug purpose
                //(if no predelay are set and blockSize is constant the size and play needle are multiple of blockSize
                if(checkForAlignment){
                jassertfalse;
                LOG("buffer not a multiple of blockSize");
                }
#endif

                int firstSegmentLength =recordNeedle - playNeedle;
                int secondSegmentLength = numSamples - firstSegmentLength;
                
                if(firstSegmentLength>0 && secondSegmentLength>0){
                    
                    const int maxChannelFromRecorded = jmin(loopSample.getNumChannels() , buffer.getNumChannels());
                    for (int i = maxChannelFromRecorded - 1; i >= 0; --i) {
                        buffer.copyFrom(i, 0, loopSample, i, playNeedle, firstSegmentLength);
                        buffer.copyFrom(i, 0, loopSample, i, 0, secondSegmentLength);
                    }
                    playNeedle = secondSegmentLength;
                }
                else{
                    jassertfalse;
                }
                
                
            }
            else{
                const int maxChannelFromRecorded = jmin(loopSample.getNumChannels() , buffer.getNumChannels());
                for (int i = maxChannelFromRecorded - 1; i >= 0; --i) {
                    buffer.copyFrom(i, 0, loopSample, i, playNeedle, numSamples);
                }
            }
        }
        
        // revert to beginning after reading last block of stopped
      if(state==BUFFER_STOPPED){playNeedle = 0;startJumpNeedle = 0;}
        else{
            
            playNeedle += numSamples;
            if(playNeedle>=recordNeedle){
                numTimePlayed ++;
            }
            playNeedle %= recordNeedle;
#ifdef BLOCKSIZEGRANULARITY
          if(checkForAlignment && numSamples>0){
          jassert(playNeedle%numSamples==0);
          jassert(recordNeedle%numSamples==0);
          }
#endif
        }
        
        
        
        
    }
    
    
    void setPlayNeedle(int n){
        if(playNeedle!=n){
            if(!isJumping){
                startJumpNeedle = playNeedle;
            }
            isJumping = true;
        }
        
        playNeedle = n;
        
        
    }
    
    void cropEndOfRecording(int sampletoRemove){
        jassert(sampletoRemove<recordNeedle);
        recordNeedle-=sampletoRemove;
    }
    
    void fadeInOut(int fadeNumSamples,double mingain){
        if (fadeNumSamples>0 ){
            
            if(recordNeedle<2 * fadeNumSamples -1) {fadeNumSamples = recordNeedle/2 - 1;}
            for (int i = loopSample.getNumChannels() - 1; i >= 0; --i) {
                loopSample.applyGainRamp(i, 0, fadeNumSamples, (float)mingain, 1);
                loopSample.applyGainRamp(i, recordNeedle - fadeNumSamples, fadeNumSamples, 1, (float)mingain);
            }
        }
    }
    bool isFirstPlayingFrameAfterRecord(){return lastState == BUFFER_RECORDING && state == BUFFER_PLAYING;}
    bool isFirstStopAfterRec(){return lastState == BUFFER_RECORDING && state == BUFFER_STOPPED;}
    bool isFirstPlayingFrame(){return lastState!=BUFFER_PLAYING && state == BUFFER_PLAYING;}
    bool isFirstRecordingFrame(){return lastState!=BUFFER_RECORDING && state == BUFFER_RECORDING;}
    bool isStopping() const{return (lastState == BUFFER_PLAYING  ) && (state==BUFFER_STOPPED);}
    bool isRecording() const{return state == BUFFER_RECORDING;}
    bool firstRecordedFrame() const{return state == BUFFER_RECORDING && (lastState!=BUFFER_RECORDING);}
    void startRecord(){recordNeedle = 0;playNeedle=0;}
    bool isOrWasPlaying() const{return (state==BUFFER_PLAYING || lastState==BUFFER_PLAYING) &&  recordNeedle>0 && loopSample.getNumSamples();}
    void startPlay(){setPlayNeedle(0);}
    
    bool checkTimeAlignment(uint64 curTime,const int minQuantifiedFraction){
        
        if(state == BUFFER_PLAYING  && recordNeedle>0){
            
            
            int globalPos =(curTime%minQuantifiedFraction);
            int localPos =(playNeedle%minQuantifiedFraction);
            if(globalPos!=localPos){
                if(!isJumping)startJumpNeedle = playNeedle;
                playNeedle = (playNeedle - localPos) + globalPos;
                isJumping = true;
                
                
            }
        }
        
        return !isJumping;
    }
    
    
    enum BufferState {
        BUFFER_STOPPED = 0,
        BUFFER_PLAYING,
        BUFFER_RECORDING
        
    };
    
    void setState(BufferState newState){
        //        lastState = state;
        stateChanged |=newState!=state;
        switch (newState){
            case BUFFER_RECORDING:
                recordNeedle = 0;
                numTimePlayed = 0;
                startJumpNeedle = 0;
                setPlayNeedle(0);
                break;
            case BUFFER_PLAYING:
                setPlayNeedle( 0);
                break;
            case BUFFER_STOPPED:
                numTimePlayed = 0;
                setPlayNeedle(0);
                break;
        }
        state = newState;
        
    }
    
    void endProcessBlock(){
        lastState = state;
        stateChanged =false;
        isJumping = false;
        startJumpNeedle=0;
    }
    
    BufferState getState() const{
        return state;
    }
    
    BufferState getLastState() const{
        return lastState;
    }
    
    
    uint64 getRecordedLength() const{return recordNeedle;}
    
    uint64 getPlayPos() const{return playNeedle;}
    
    
    bool stateChanged;
    
    uint64 getStartJumpPos() const{return startJumpNeedle;}
    
    
    
    
    int numTimePlayed;
    AudioSampleBuffer loopSample;
    
private:
    
    BufferState state;
    BufferState lastState;
    bool isJumping;
    
    
    
    uint64 recordNeedle,playNeedle,startJumpNeedle;
    
};



#endif  // PLAYABLEBUFFER_H_INCLUDED
