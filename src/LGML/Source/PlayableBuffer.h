/*
 ==============================================================================

 PlayableBuffer.h
 Created: 6 Jun 2016 7:45:50pm
 Author:  Martin Hermant

 ==============================================================================
 */

#ifndef PLAYABLEBUFFER_H_INCLUDED
#define PLAYABLEBUFFER_H_INCLUDED
#pragma once


#include "AudioHelpers.h"

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
  numTimePlayed(0),
  sampleOffsetBeforeNewState(0),
  hasBeenFaded (false),fadeSamples(380),
  fadeRecorded(1000)
  {
  
    fadeRecorded.setFadedOut();
    jassert(numSamples < std::numeric_limits<int>::max());
    //        for (int j = 0 ; j < numSamples ; j++){int p = 44;float t = (j%p)*1.0/p;float v = t;
    //            for(int i = 0 ; i < numChannels ; i++){loopSample.addSample(i, j, v);}
    //        }
//    loopSample.clear();
  }


  void setNumChannels(int n){
    loopSample.setSize(n, loopSample.getNumSamples());
  }
  bool processNextBlock(AudioBuffer<float> & buffer){
    bool succeeded = true;
    if (isFirstRecordingFrame()){
      succeeded = writeAudioBlock(buffer, sampleOffsetBeforeNewState);

    }
    else if(isRecording() ){
      succeeded = writeAudioBlock(buffer);
    }
    else if( wasLastRecordingFrame()){
      succeeded = writeAudioBlock(buffer, 0,sampleOffsetBeforeNewState);
      fadeInOut(fadeSamples, 0);
    }



      buffer.clear();

    if(isStopping()){
      fadeRecorded.startFadeOut();
    }
    if(isFirstPlayingFrame()){

      fadeRecorded.setFadedOut();
      fadeRecorded.startFadeIn();
    }






fadeRecorded.incrementFade(buffer.getNumSamples());
    if ( isOrWasPlaying()||fadeRecorded.getLastFade()>0){
//    if(isOrWasPlaying()){
      readNextBlock(buffer,sampleOffsetBeforeNewState);
//    }

      double startGain = fadeRecorded.getLastFade();
      double endGain = fadeRecorded.getCurrentFade();
      const int maxChannel = jmin(loopSample.getNumChannels(),buffer.getNumChannels());
      for(int c = 0 ; c <maxChannel ; c++ ){
        buffer.applyGainRamp(c, 0, buffer.getNumSamples(), (float)startGain, (float)endGain);
      }

    }





    return succeeded;
  }


  inline bool writeAudioBlock(const AudioBuffer<float> & buffer, int fromSample = 0,int samplesToWrite = -1){

    samplesToWrite= samplesToWrite==-1?buffer.getNumSamples()-fromSample:samplesToWrite;
    if (recordNeedle + buffer.getNumSamples()> loopSample.getNumSamples()) {
//      jassertfalse;
      return false;
    }
    else{
      const int maxChannel = jmin(loopSample.getNumChannels(),buffer.getNumChannels());
      for (int i =  maxChannel- 1; i >= 0; --i) {
        loopSample.copyFrom(i, (int)recordNeedle, buffer, i, fromSample, samplesToWrite );
      }
      recordNeedle += samplesToWrite;
    }

    return true;
  }


  inline void readNextBlock(AudioBuffer<float> & buffer,int fromSample = 0  ){
    if(recordNeedle==0){
      buffer.clear();
      jassertfalse;
      return;
    }
//    jassert(isOrWasPlaying());

    int numSamples = buffer.getNumSamples()-fromSample;


    // we want to read Last Block for fade out if stopped
//    if(state==BUFFER_STOPPED){
//      playNeedle = startJumpNeedle;
//    }

    // assert false for now to check alignement
    if(isFirstPlayingFrame()){
      jassert(playNeedle==0);
    }


    // stitch audio jumps by quick fadeIn/Out
    if(isJumping && playNeedle!=startJumpNeedle && state!=BUFFER_STOPPED){
      //LOG("a:jump "<<startJumpNeedle <<","<< playNeedle);

      const int halfBlock =  numSamples/2;
      for (int i = buffer.getNumChannels() - 1; i >= 0; --i) {
        const int maxChannelFromRecorded = jmin(loopSample.getNumChannels() - 1, i);
        buffer.copyFrom(i, fromSample, loopSample, maxChannelFromRecorded, (int)startJumpNeedle, halfBlock);
        buffer.applyGainRamp(i, fromSample, halfBlock, 1.0f, 0.0f);
        buffer.copyFrom(i, fromSample+halfBlock, loopSample, maxChannelFromRecorded, (int)playNeedle+halfBlock, halfBlock);
        buffer.applyGainRamp(i, fromSample+halfBlock-1, halfBlock, 0.0f, 1.0f);

      }
    }

    else{
      if ((playNeedle + numSamples) > recordNeedle)
      {


        int firstSegmentLength =(int)(recordNeedle - playNeedle);
        int secondSegmentLength = numSamples - firstSegmentLength;

        if(firstSegmentLength>0 && secondSegmentLength>0){

          const int maxChannelFromRecorded = jmin(loopSample.getNumChannels() , buffer.getNumChannels());
          for (int i = maxChannelFromRecorded - 1; i >= 0; --i) {
            buffer.copyFrom(i, fromSample, loopSample, i, (int)playNeedle, firstSegmentLength);
            buffer.copyFrom(i, fromSample+firstSegmentLength, loopSample, i, 0, secondSegmentLength);
          }

        }
        else{jassertfalse;}


      }
      else{
        const int maxChannelFromRecorded = jmin(loopSample.getNumChannels() , buffer.getNumChannels());
        for (int i = maxChannelFromRecorded - 1; i >= 0; --i) {
          buffer.copyFrom(i, fromSample, loopSample, i, (int)playNeedle, numSamples);
        }
      }
    }

    // revert to beginning after reading last block of stopped
//    if(state==BUFFER_STOPPED){playNeedle = 0;startJumpNeedle = 0;}
//    else{

      playNeedle += numSamples;
    globalPlayNeedle+=numSamples;
      if(playNeedle>=recordNeedle){
        numTimePlayed ++;
      }
      playNeedle %= recordNeedle;

//    }

  }


  void setPlayNeedle(int n){
    if(playNeedle!=n){
      if(!isJumping){
        startJumpNeedle = playNeedle;
      }
      isJumping = true;
    }

    playNeedle = n;
    //        std::cout << playNeedle << " : " << std::hex << this << std::endl;


  }

  void cropEndOfRecording(int sampletoRemove){
    jassert(sampletoRemove<recordNeedle);
    recordNeedle-=sampletoRemove;
  }
  void padEndOfRecording(int sampleToAdd){
    loopSample.clear((int)recordNeedle, sampleToAdd);
    recordNeedle+=sampleToAdd;
  }
  void setSizePaddingIfNeeded(uint64 targetSamples){
    jassert(targetSamples<loopSample.getNumSamples());
    if(targetSamples>recordNeedle){
      padEndOfRecording((int)(targetSamples - recordNeedle));
    }
    else if (targetSamples<recordNeedle){
      cropEndOfRecording((int)(recordNeedle - targetSamples));
    }

  }

  void fadeInOut(int fadeNumSamples,double mingain){
    if (fadeNumSamples>0 ){
      if(recordNeedle<2 * fadeNumSamples -1) {fadeNumSamples = (int)recordNeedle/2 - 1;}
      for (int i = loopSample.getNumChannels() - 1; i >= 0; --i) {
        loopSample.applyGainRamp(i, 0, fadeNumSamples, (float)mingain, 1);
        loopSample.applyGainRamp(i, (int)recordNeedle - fadeNumSamples, fadeNumSamples, 1, (float)mingain);
      }
    }
  }
  inline bool isFirstPlayingFrameAfterRecord()const{return lastState == BUFFER_RECORDING && state == BUFFER_PLAYING;}
  inline bool isFirstStopAfterRec()const{return lastState == BUFFER_RECORDING && state == BUFFER_STOPPED;}
  inline bool isFirstPlayingFrame()const{return lastState!=BUFFER_PLAYING && state == BUFFER_PLAYING;}
  inline bool isFirstRecordingFrame()const{return lastState!=BUFFER_RECORDING && state == BUFFER_RECORDING;}
  inline bool wasLastRecordingFrame()const{return lastState==BUFFER_RECORDING && state != BUFFER_RECORDING;}
  inline bool isStopping() const{return (lastState != BUFFER_STOPPED  ) && (state==BUFFER_STOPPED);}
  inline bool isStopped() const{return (state==BUFFER_STOPPED);}
  inline bool isRecording() const{return state == BUFFER_RECORDING;}
  inline bool isPlaying() const{return state == BUFFER_PLAYING;}
  inline bool isFirstRecordedFrame() const{return state == BUFFER_RECORDING && (lastState!=BUFFER_RECORDING);}
  inline bool isOrWasPlaying() const{return (state==BUFFER_PLAYING || lastState==BUFFER_PLAYING) &&  recordNeedle>0 && loopSample.getNumSamples();}
  inline bool isOrWasRecording() const{return (state==BUFFER_RECORDING || lastState==BUFFER_RECORDING) && loopSample.getNumSamples();}


  void startRecord(){recordNeedle = 0;playNeedle=0;}
  inline void startPlay(){setPlayNeedle(0);}

  bool checkTimeAlignment(uint64 curTime,const int minQuantifiedFraction){

    if(state == BUFFER_PLAYING  && recordNeedle>0){


      int globalPos =(curTime%minQuantifiedFraction);
      int localPos =(playNeedle%minQuantifiedFraction);
      if(globalPos!=localPos){
        if(!isJumping)startJumpNeedle = playNeedle;
        playNeedle = (playNeedle - localPos) + globalPos;
        isJumping = true;
        jassertfalse;


      }
    }

    return !isJumping;
  }




  enum BufferState {
    BUFFER_STOPPED = 0,
    BUFFER_PLAYING,
    BUFFER_RECORDING

  };

  void setState(BufferState newState,int _sampleOffsetBeforeNewState=0){
    //        lastState = state;
    stateChanged |=newState!=state;
    switch (newState){
      case BUFFER_RECORDING:
        hasBeenFaded = false;
        recordNeedle = 0;
        numTimePlayed = 0;
        startJumpNeedle = 0;
        setPlayNeedle(0);
        globalPlayNeedle = 0;
        break;
      case BUFFER_PLAYING:
        fadeRecorded.startFadeIn();
        setPlayNeedle( 0);
        break;
      case BUFFER_STOPPED:
        numTimePlayed = 0;
        fadeRecorded.startFadeOut();
        globalPlayNeedle = 0;
//        setPlayNeedle(0);
        break;
    }
    state = newState;
    sampleOffsetBeforeNewState = _sampleOffsetBeforeNewState;
  }

  void endProcessBlock(){
    lastState = state;
    stateChanged =false;
    isJumping = false;
    startJumpNeedle=playNeedle;
    sampleOffsetBeforeNewState = 0;
  }

  BufferState getState() const{
    return state;
  }

  BufferState getLastState() const{
    return lastState;
  }


  uint64 getRecordedLength() const{return recordNeedle;}

  uint64 getPlayPos() const{return playNeedle;}
  uint64 getGlobalPlayPos() const{return globalPlayNeedle;}


  bool stateChanged;

  uint64 getStartJumpPos() const{return startJumpNeedle;}




  int numTimePlayed;
  AudioSampleBuffer loopSample;

  int getSampleOffsetBeforeNewState(){return sampleOffsetBeforeNewState;};

  int getNumSampleFadeOut(){return fadeRecorded.fadeOutNumSamples;};

#if !LGML_UNIT_TESTS
private:
#endif
  
  int sampleOffsetBeforeNewState;
  BufferState state;
  BufferState lastState;
  bool isJumping;
  bool hasBeenFaded;
  int fadeSamples;
  FadeInOut fadeRecorded;
  
  
  
  uint64 recordNeedle,playNeedle,startJumpNeedle,globalPlayNeedle;
  
};



#endif  // PLAYABLEBUFFER_H_INCLUDED
