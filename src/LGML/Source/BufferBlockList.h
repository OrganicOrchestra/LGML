/*
  ==============================================================================

    BufferBlockList.h
    Created: 15 Jul 2017 3:05:20pm
    Author:  Martin Hermant

  ==============================================================================
*/

#pragma once
#include "JuceHeader.h"

#define DEFAULT_BUFFER_BLOCK_SIZE 10000 //1000000 // ~ 20s@44.1kHz should be bigger than the highest block size

class BufferBlockList : public Array<AudioSampleBuffer>{
public:
  BufferBlockList(int numChannels=1,int minNumSample= 44100*2,int blockSize = DEFAULT_BUFFER_BLOCK_SIZE );
  void allocateSamples(int numChannels,int numSamples );
  int getAllocatedNumSample() const;
  int getNumSamples() const;
  int getAllocatedNumChannels() const;
  void setNumChannels(int numChannels);
  void setNumSample(int numSample);
  void copyTo(AudioSampleBuffer & outBuf,int listStartSample,int bufStartSample=0,int numSampleToCopy=-1);
  void copyFrom(const AudioSampleBuffer & inBuf,int listStartSample,int bufStartSample=0,int numSampleToCopy=-1);
  float getSample(int c, int n);
  AudioSampleBuffer & getContiguousBuffer(int sampleStart=0,int numSamples=-1);
  AudioSampleBuffer contiguous_Cache;
  int targetNumSamples;
  int minNumSample;
  int bufferBlockSize;

};
