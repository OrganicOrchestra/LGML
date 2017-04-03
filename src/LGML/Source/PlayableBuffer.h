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

#include "AudioConfig.h"
#include "AudioHelpers.h"




#include "MultiNeedle.h"

#if BUFFER_CAN_STRETCH
class StretcherJob;
namespace RubberBand{class RubberBandStretcher;};
#define RT_STRETCH 0 // TODO : still WIP
#else
#define RT_STRETCH 0
#endif

class PlayableBuffer {

  public :
  PlayableBuffer(int numChannels,int numSamples,int sampleRate,int blockSize);
  ~PlayableBuffer();
  void setNumChannels(int n);
  bool processNextBlock(AudioBuffer<float> & buffer,uint64 time);


  bool writeAudioBlock(const AudioBuffer<float> & buffer, int fromSample = 0,int samplesToWrite = -1,bool isTail=false);
  void readNextBlock(AudioBuffer<float> & buffer,uint64 time,int fromSample = 0  );


  void setPlayNeedle(int n);

  void cropEndOfRecording(int * sampletoRemove);
  void padEndOfRecording(int sampleToAdd);
  void setRecordedLength(uint64 targetSamples);

  
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
  bool isRecordingTail() const;

  void stopRecordingTail();
  


  void startRecord();
  inline void startPlay();



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

  




  int numTimePlayed;
  AudioSampleBuffer audioBuffer,originAudioBuffer;
  MultiNeedle multiNeedle;

  int getSampleOffsetBeforeNewState();
  int getNumSampleFadeOut() const;


#if BUFFER_CAN_STRETCH
  void setTimeRatio(const double ratio);
#endif
  void setSampleRate(float sR);
  float sampleRate;


#if !LGML_UNIT_TESTS
private:
#endif


  ////
  //stretch

#if RT_STRETCH
  void initRTStretch(int blockSize);
  void applyStretch();
  void processPendingRTStretch(AudioBuffer<float> & b);
  ScopedPointer<RubberBand::RubberBandStretcher> RTStretcher;
  float pendingTimeStretchRatio;

#endif

#if BUFFER_CAN_STRETCH
  friend class StretcherJob;
  StretcherJob *stretchJob;
#endif

  int sampleOffsetBeforeNewState;
  BufferState state;
  BufferState lastState;
  
  








  uint64 recordNeedle,playNeedle,globalPlayNeedle;

  
  int tailRecordNeedle;



  JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(PlayableBuffer);

};



#endif  // PLAYABLEBUFFER_H_INCLUDED
