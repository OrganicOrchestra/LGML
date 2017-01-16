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
namespace RubberBand{class RubberBandStretcher;}

class PlayableBuffer {

  public :
  PlayableBuffer(int numChannels,int numSamples,int sampleRate,int blockSize);
  ~PlayableBuffer();
  void setNumChannels(int n);
  bool processNextBlock(AudioBuffer<float> & buffer);


  inline bool writeAudioBlock(const AudioBuffer<float> & buffer, int fromSample = 0,int samplesToWrite = -1);
  inline void readNextBlock(AudioBuffer<float> & buffer,int fromSample = 0  );


  void setPlayNeedle(int n);

  void cropEndOfRecording(int sampletoRemove);
  void padEndOfRecording(int sampleToAdd);
  void setSizePaddingIfNeeded(uint64 targetSamples);

  void fadeInOut(int fadeNumSamples,double mingain);
  bool isFirstPlayingFrameAfterRecord()const;
  bool isFirstStopAfterRec()const;
  bool isFirstPlayingFrame()const;
  bool isFirstRecordingFrame()const;
  bool wasLastRecordingFrame()const;
  bool isStopping() const;
  bool isStopped() const;
  bool isRecording() const;
  bool isPlaying() const;
  bool isFirstRecordedFrame() const;
  bool isOrWasPlaying() const;
  bool isOrWasRecording() const;


  void startRecord();
  inline void startPlay();

  bool checkTimeAlignment(uint64 curTime,const int minQuantifiedFraction);




  enum BufferState {
    BUFFER_STOPPED = 0,
    BUFFER_PLAYING,
    BUFFER_RECORDING

  };

  void setState(BufferState newState,int _sampleOffsetBeforeNewState=0);

  void endProcessBlock();

  BufferState getState() const;
  BufferState getLastState() const;


  uint64 getRecordedLength() const;
  uint64 getStretchedLength() const;

  uint64 getPlayPos() const;
  uint64 getGlobalPlayPos() const;


  bool stateChanged;

  uint64 getStartJumpPos() const;




  int numTimePlayed;
  AudioSampleBuffer loopSample,originLoopSample;

  int getSampleOffsetBeforeNewState();
  int getNumSampleFadeOut();


  // stretching function
  void initStretcher(int sR,int c,int blockSize);
  void setTimeRatio(const double ratio);
  CriticalSection stretcherLock;


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
  
  

  ScopedPointer<RubberBand::RubberBandStretcher> stretcher;


  uint64 recordNeedle,playNeedle,startJumpNeedle,globalPlayNeedle;







  
  JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(PlayableBuffer);
  
};



#endif  // PLAYABLEBUFFER_H_INCLUDED
