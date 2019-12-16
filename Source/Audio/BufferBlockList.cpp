/*
 ==============================================================================

 Copyright © Organic Orchestra, 2017

 This file is part of LGML. LGML is a software to manipulate sound in real-time

 This program is free software; you can redistribute it and/or modify
 it under the terms of the GNU General Public License as published by
 the Free Software Foundation (version 3 of the License).

 This program is distributed in the hope that it will be useful,
 but WITHOUT ANY WARRANTY; without even the implied warranty of
 MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.

 ==============================================================================
 */

#include "BufferBlockList.h"



BufferBlockList::BufferBlockList (int _numChannels, int  _minNumSample, int _blockSize):
minNumSample (_minNumSample),
bufferBlockSize (_blockSize)
{
    jassert (bufferBlockSize > 0);
    allocateSamples (_numChannels, minNumSample);

}

void BufferBlockList::allocateSamples (int numChannels, int numSamples,bool dontShrink)
{
    jassert (numChannels > 0);

    //  jassert(numSamples>0);
    if (getAllocatedNumChannels() != numChannels)
    {
        for (int i = 0 ; i < size() ; i++)
        {
            OwnedArray::getUnchecked (i)->setSize (numChannels, bufferBlockSize,
                                                   true // keep existing content
                                                   ,true // zero extra space
                                                   ,true // avoid reallocating);
                                                   );
        }
    }

    int i = size() * bufferBlockSize;

    numSamples = jmax (numSamples, minNumSample);

    while (i <= numSamples)
    {
        add (new AudioSampleBuffer (numChannels, bufferBlockSize));
        i += bufferBlockSize;
    }
    if(!dontShrink){
        while (i > numSamples + bufferBlockSize)
        {
            removeLast();
            i -= bufferBlockSize;
            
        }
    }

}

void BufferBlockList::setNumChannels (int numChannels)
{
    for (auto c : *this)
    {
        c->setSize (numChannels, bufferBlockSize,
                    true // keep existing content
                    ,true // zero extra space
                    ,true // avoid reallocating
                    );
    }
}
void BufferBlockList::setNumSample (int numSamples)
{
    allocateSamples (getAllocatedNumChannels(), numSamples);
    targetNumSamples = numSamples;
}
int BufferBlockList::getNumSamples() const
{
    return targetNumSamples;
}
int BufferBlockList::getAllocatedNumSample()const
{
    return size() * bufferBlockSize;
}
int BufferBlockList::getAllocatedNumChannels()const
{
    return size() > 0 ? OwnedArray::getFirst()->getNumChannels() : 0;
}
void BufferBlockList::copyTo (AudioSampleBuffer& outBuf, int listStartSample, int bufStartSample, int numSampleToCopy) const
{
    if (numSampleToCopy == -1) numSampleToCopy = outBuf.getNumSamples();

    jassert (numSampleToCopy <= size()*bufferBlockSize);
    int numChannels = jmin (getAllocatedNumChannels(), outBuf.getNumChannels());
    int sampleProcessed = 0;
    int readPosInList = listStartSample;
    int readBlockIdx;
    int writePos = bufStartSample;

    while (sampleProcessed < numSampleToCopy)
    {
        readBlockIdx = (int)floor (readPosInList * 1.0 / bufferBlockSize);
        int readPosInBlock = (readPosInList % targetNumSamples) % bufferBlockSize ;

        int blockSize =  bufferBlockSize - readPosInBlock;

        if (sampleProcessed + blockSize > numSampleToCopy)
            blockSize =  numSampleToCopy - sampleProcessed;

        jassert (blockSize > 0);
        jassert (readPosInBlock + blockSize <= bufferBlockSize);
        jassert (readBlockIdx < size());

        for (int i = 0 ; i < numChannels ; i++)
        {
            const auto ref = OwnedArray::getUnchecked (readBlockIdx);
            outBuf.copyFrom ( i, writePos, *ref, i, readPosInBlock, blockSize);
        }

        readPosInList += blockSize;
        writePos += blockSize;
        sampleProcessed += blockSize;

        if (readPosInList >= targetNumSamples)
        {
            jassert (readPosInList < targetNumSamples + bufferBlockSize);
            readPosInList %= targetNumSamples;
        }



    }

}

void BufferBlockList::copyFrom (const AudioSampleBuffer& inBuf, int listStartSample, int bufStartSample, int numSampleToCopy)
{
    if (numSampleToCopy == -1)numSampleToCopy =  inBuf.getNumSamples();

    jassert (listStartSample + numSampleToCopy < getAllocatedNumSample());
    jassert (inBuf.getNumChannels() == getAllocatedNumChannels());
    int numChannels = inBuf.getNumChannels();
    int sampleProcessed = 0;
    int readPos = bufStartSample;

    int writeBlockIdx ;
    int writePosInList = listStartSample;

    while (sampleProcessed < numSampleToCopy)
    {
        writeBlockIdx = (int)floor (writePosInList * 1.0 / bufferBlockSize);
        int startWrite = writePosInList % bufferBlockSize ;
        int blockSize =  bufferBlockSize - startWrite;

        if (sampleProcessed + blockSize > numSampleToCopy)
            blockSize =  numSampleToCopy - sampleProcessed;

        jassert (blockSize > 0);
        jassert (startWrite + blockSize <= bufferBlockSize);
        jassert (writeBlockIdx < size());

        for (int i = 0 ; i < numChannels ; i++)
        {
            auto* ref =  OwnedArray::getUnchecked (writeBlockIdx);
            ref->copyFrom (i, startWrite, inBuf, i, readPos, blockSize);

        }

        readPos += blockSize;
        writePosInList += blockSize;
        sampleProcessed += blockSize;

        jassert (writePosInList < getAllocatedNumSample());

    }
}

float BufferBlockList::getSample (int c, int n)
{
    int readI = n % bufferBlockSize ;
    auto readBI = (int)floor (n * 1.0 / bufferBlockSize);
    jassert (readBI < size());
    return  OwnedArray::getUnchecked (readBI)->getSample (c, readI);
}


AudioSampleBuffer& BufferBlockList::getContiguousBuffer (int sampleStart, int numSamples)
{
    if (numSamples == -1)numSamples = getNumSamples() - sampleStart;

    contiguous_Cache.setSize (getAllocatedNumChannels(), numSamples);
    copyTo (contiguous_Cache, sampleStart, 0, numSamples);
    return contiguous_Cache;
}
AudioSampleBuffer& BufferBlockList::fillAll (AudioSampleBuffer& buf) const
{
    auto numSamples = getNumSamples() ;

    buf.setSize (getAllocatedNumChannels(), numSamples);
    copyTo (buf, 0, 0, numSamples);
    return buf;
}
