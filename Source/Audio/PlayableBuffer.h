/* Copyright © Organic Orchestra, 2017
*
* This file is part of LGML.  LGML is a software to manipulate sound in realtime
*
* This program is free software; you can redistribute it and/or modify
* it under the terms of the GNU General Public License as published by
* the Free Software Foundation (version 3 of the License).
*
* This program is distributed in the hope that it will be useful,
* but WITHOUT ANY WARRANTY; without even the implied warranty of
* MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
*
*/


#ifndef PLAYABLEBUFFER_H_INCLUDED
#define PLAYABLEBUFFER_H_INCLUDED
#pragma once

#include "AudioConfig.h"
#include "AudioHelpers.h"
#include "BufferBlockList.h"



#include "MultiNeedle.h"
 

#if BUFFER_CAN_STRETCH
class StretcherJob;
namespace RubberBand{class RubberBandStretcher;};
#define RT_STRETCH 1 // TODO : validate
#else
#define RT_STRETCH 0
#endif




class PlayableBuffer {

  public :
  PlayableBuffer(int numChannels,int numSamples,float sampleRate,int blockSize);
  ~PlayableBuffer();
  void setNumChannels(int n);
  int getNumChannels()const;
  int getAllocatedNumSample() const;
  bool processNextBlock(AudioBuffer<float> & buffer,sample_clk_t time);


  bool writeAudioBlock(const AudioBuffer<float> & buffer, int fromSample = 0,int samplesToWrite = -1);
  void readNextBlock(AudioBuffer<float> & buffer,sample_clk_t time,int fromSample = 0  );


  void setPlayNeedle(int n);

  void cropEndOfRecording(int * sampletoRemove);
//  void padEndOfRecording(int sampleToAdd);
  void setRecordedLength(sample_clk_t targetSamples);

  
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
//  bool isRecordingTail() const;
//
//  void stopRecordingTail();



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


  sample_clk_t getRecordedLength() const;
  int getMinRecordSampleLength() const;
  sample_clk_t getStretchedLength() const;

  sample_clk_t getPlayPos() const;
  sample_clk_t getGlobalPlayPos() const;


  bool stateChanged;

  




  int numTimePlayed;
  AudioSampleBuffer originAudioBuffer;
  BufferBlockList bufferBlockList;
  MultiNeedle multiNeedle;

  int getSampleOffsetBeforeNewState();
  int getNumSampleFadeOut() const;


#if BUFFER_CAN_STRETCH
  void setTimeRatio(const double ratio);
#endif
  void setSampleRate(float sR);
  float sampleRate;
  int blockSize;
  void setBlockSize(int bs);

//  void findFadeLoopPoints();
//  uint64 fadeLoopOutPoint;
//  bool reverseFadeOut;
  Array<int> onsetSamples;

#if !LGML_UNIT_TESTS
private:
#endif


  ////
  //stretch

#if RT_STRETCH
  void initRTStretch();
  void applyStretch();
  bool processPendingRTStretch(AudioBuffer<float> & b,sample_clk_t time);
  ScopedPointer<RubberBand::RubberBandStretcher> RTStretcher;
  float pendingTimeStretchRatio;
  int processedStretch;
  int stretchNeedle;

  FadeInOut fadePendingStretch;

#endif
  bool isStretchPending;
#if BUFFER_CAN_STRETCH
  friend class StretcherJob;
  StretcherJob *stretchJob;
  AudioSampleBuffer tmpBufferStretch;
  bool isStretchReady;

#endif

  int sampleOffsetBeforeNewState;
  BufferState state;
  BufferState lastState;
  
  
  void fadeInOut();







  sample_clk_t recordNeedle,playNeedle,globalPlayNeedle;

  
  int tailRecordNeedle;




  JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(PlayableBuffer);

};



#endif  // PLAYABLEBUFFER_H_INCLUDED
