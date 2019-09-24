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

#ifndef RINGBUFFER_H_INCLUDED
#define RINGBUFFER_H_INCLUDED

#pragma once


/*
 helper class for phantom buffer
 allowing having constant access to contiguous memory in a circular buffer without dealing with overlaps
 it allocate twice the size though ...
 */
class PhantomBuffer
{

public:
    PhantomBuffer (int _channels, int size, int _phantomSize = -1): numChannels (_channels)
    {
        bufSize = size;

        if (_phantomSize > 0)phantomSize = _phantomSize;
        else phantomSize = size;

        buf.setSize (numChannels, bufSize + phantomSize, false, true);
        writeNeedle = 0;
    }

    void setNumChannels (int channels)
    {
        numChannels = channels;
        buf.setSize (numChannels, bufSize + phantomSize, false, true);
    }
    void writeBlock (AudioSampleBuffer& newBuf)
    {


        jassert (newBuf.getNumChannels() >= numChannels);
        int numToChCopy = jmin(newBuf.getNumChannels(),numChannels);
        int toCopy = newBuf.getNumSamples();

        // overlap
        if ( writeNeedle + toCopy > bufSize)
        {
            int firstSeg = bufSize - (writeNeedle) ;
            jassert (firstSeg < newBuf.getNumSamples());

            for (int i = numToChCopy - 1; i >= 0 ; --i)
            {
                safeCopy (newBuf.getReadPointer (i, 0), firstSeg, i);
            }

            jassert (writeNeedle == 0);

            for (int i = numToChCopy - 1; i >= 0 ; --i)
            {
                safeCopy (newBuf.getReadPointer (i, firstSeg), toCopy - firstSeg, i);
            }

        }
        else
        {
            for (int i = numToChCopy - 1; i >= 0 ; --i)
            {
                safeCopy (newBuf.getReadPointer (i), toCopy, i);
            }

        }
    }


    const AudioBuffer<float>& getLastBlock (int num)
    {
        jassert (num <= phantomSize);
        pointers.ensureStorageAllocated (numChannels);

        for (int i = 0 ; i < numChannels ; i++)
        {
            pointers.set (i, buf.getArrayOfWritePointers()[i] + phantomSize + writeNeedle - num);
        }

        contiguousBuffer.setDataToReferTo (pointers.getRawDataPointer(), numChannels, num);
        return contiguousBuffer;
    }

    const void printContent()
    {
        String niceOutput;

        for (int i = 0 ; i < buf.getNumSamples() ; i++)
        {
            for (int c = 0 ; c < numChannels ; c++) {niceOutput += "\t  " + String (buf.getSample (c, i));}

            if (i == phantomSize + writeNeedle) {niceOutput += "\t  writeNeedle";}

            if (i % phantomSize == 0) {niceOutput += "\t  phantomLimit";}

            niceOutput += "\n";
        }

        DBG (niceOutput);
    }



    AudioSampleBuffer buf{};
private:
    void safeCopy (const float* b, int numSample, int channel)
    {

        jassert (writeNeedle + numSample <= bufSize);
        buf.copyFrom (channel, phantomSize + writeNeedle, b, numSample);

        if (writeNeedle + numSample > bufSize - phantomSize)buf.copyFrom (channel, writeNeedle, b, numSample);

        if (channel == 0 )
        {
            writeNeedle += numSample;
            writeNeedle %= bufSize;
        }

    }
    int writeNeedle;
    int phantomSize;
    int bufSize;
    int numChannels ;
    AudioBuffer<float> contiguousBuffer{};
    Array<float*> pointers{};

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (PhantomBuffer);


};




//==============================================================================
//==============================================================================
#if LGML_UNIT_TESTS

class PhantomBufferTest  : public UnitTest
{
public:
    PhantomBufferTest() : UnitTest ("RingBufferTest") {}



    void runTest() override
    {
        beginTest ("readBack");


        int numBlocks = 2;
        int blockSize = 10;
        int numChannels = 2;

        int ringSize = numBlocks * blockSize;
        int copiedSize = (numBlocks + 2) * (blockSize);


        PhantomBuffer ring (numChannels, ringSize);

        AudioBuffer<float> tstBuf;
        tstBuf.setSize (numChannels, blockSize);

        AudioBuffer<float> expected;
        expected.setSize (numChannels, ringSize);


        for (int i = 0 ; i < copiedSize ; i++)
        {

            float dstValue = i;

            for (int c = 0 ;  c < numChannels ; c++)
            {
                tstBuf.setSample (c, i % blockSize, dstValue);

            }

            if ((i + 1) % blockSize == 0)
            {
                expect (tstBuf.getMagnitude (0, tstBuf.getNumSamples()) > 0, "setting empty buffer");
                ring.writeBlock (tstBuf);
            }

            if (i >= (copiedSize - ringSize))
            {
                for (int c = 0 ;  c < numChannels ; c++)
                {
                    expected.setSample (c, i - (copiedSize - ringSize), dstValue);
                }
            }


        }


        // reading last
        int blockNum = ceil (ringSize / blockSize);
        const AudioBuffer<float>  b = ring.getLastBlock (blockNum * blockSize);

        for (int c = 0  ; c < numChannels ; c++)
        {
            for ( int j = 0 ; j < blockNum * blockSize ; j++)
            {
                float fexpected =  expected.getSample (c, j);
                float found = b.getSample (c, j);
                String err = "failed at :" + String (j) + ",expect: " + String (fexpected) + "found: " + String (found) ;
                expect (found == fexpected, err);

            }
        }

    }
};

static PhantomBufferTest ringBufferTest;

#endif


#endif  // RINGBUFFER_H_INCLUDED
