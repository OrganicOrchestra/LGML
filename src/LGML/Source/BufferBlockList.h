/*
  ==============================================================================

    BufferBlockList.h
    Created: 15 Jul 2017 3:05:20pm
    Author:  Martin Hermant

  ==============================================================================
*/

#pragma once
#include "JuceHeader.h"

#define BUFFER_BLOCK_SIZE 8192 // should be bigger than the highest block size

class BufferBlockList : public Array<AudioSampleBuffer>{
public:
  void allocateSamples(int numChannels,int numSamples );
  int getAllocatedNumSample() const;
  int getNumSamples() const;
  int getAllocatedNumChannels() const;
  void setNumChannels(int numChannels);
  void setNumSample(int numSample);
  void copyTo(AudioSampleBuffer & outBuf,int sampleStart,int numSampleToCopy);
  void copyFrom(const AudioSampleBuffer & inBuf,int sampleStart,int numSampleToCopy = -1);
  AudioSampleBuffer & getContiguousBuffer(int sampleStart,int numSamples);
  AudioSampleBuffer contiguous_Cache;
  int targetNumSamples;

};
