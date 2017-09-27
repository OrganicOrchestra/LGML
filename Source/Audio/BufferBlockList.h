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

#pragma once
#include "../JuceHeaderAudio.h"

#define DEFAULT_BUFFER_BLOCK_SIZE 10000 //1000000 // ~ 20s@44.1kHz should be bigger than the highest block size

class BufferBlockList : public OwnedArray<AudioSampleBuffer>
{
public:
    BufferBlockList (int numChannels = 1, int minNumSample = 44100 * 2, int blockSize = DEFAULT_BUFFER_BLOCK_SIZE );
    void allocateSamples (int numChannels, int numSamples );
    int getAllocatedNumSample() const;
    int getNumSamples() const;
    int getAllocatedNumChannels() const;
    void setNumChannels (int numChannels);
    void setNumSample (int numSample);
    void copyTo (AudioSampleBuffer& outBuf, int listStartSample, int bufStartSample = 0, int numSampleToCopy = -1);
    void copyFrom (const AudioSampleBuffer& inBuf, int listStartSample, int bufStartSample = 0, int numSampleToCopy = -1);
    float getSample (int c, int n);
    AudioSampleBuffer& getContiguousBuffer (int sampleStart = 0, int numSamples = -1);
    AudioSampleBuffer contiguous_Cache;
    int targetNumSamples;
    int minNumSample;
    int bufferBlockSize;

};
