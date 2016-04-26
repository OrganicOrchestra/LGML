/*
 ==============================================================================

 AudioHelpers.h
 Created: 8 Mar 2016 12:33:13pm
 Author:  Martin Hermant

 ==============================================================================
 */

#ifndef AUDIOHELPERS_H_INCLUDED
#define AUDIOHELPERS_H_INCLUDED


#include "JuceHeader.h"

/*
 helper class for bipartite buffer
 allowing having constant access to contiguous memory in a circular buffer
 */
class BipBuffer{

public:
    BipBuffer(int size){
        phantomSize = size;
        buf.setSize(1,3*size);
        writeNeedle = 0;
    }

    void writeBlock(AudioSampleBuffer & newBuf){

        buf.setSize(newBuf.getNumChannels(),buf.getNumSamples());


        int toCopy = newBuf.getNumSamples();

        if( phantomSize+writeNeedle + toCopy > 3*phantomSize){
            int firstSeg = 3*phantomSize-(phantomSize+writeNeedle) ;
            for(int i = newBuf.getNumChannels()-1;i>=0 ;--i){
                safeCopy(newBuf.getReadPointer(i,0),firstSeg,i);
            }
            for(int i = newBuf.getNumChannels()-1;i>=0 ;--i){
                safeCopy(newBuf.getReadPointer(i,firstSeg),toCopy-firstSeg,i);
            }
        }
        else{
            for(int i = newBuf.getNumChannels()-1;i>=0 ;--i){
                safeCopy(newBuf.getReadPointer(i),toCopy,i);
            }
        }
    }


    const float* getLastBlock(int num,int channel=0){
        jassert(num<phantomSize);
        return buf.getReadPointer(channel, phantomSize + writeNeedle - num);
    }

    AudioSampleBuffer buf;
private:
    void safeCopy(const float * b,int s,int channel){
        buf.copyFrom(channel, phantomSize+writeNeedle, b, s);

        if(writeNeedle>2*phantomSize){
            buf.copyFrom(channel, writeNeedle-2*phantomSize,b,s);
        }
        writeNeedle+=s;
        writeNeedle%=2*phantomSize;

    }
    int writeNeedle;
    int phantomSize;




};


class RingBuffer{
public:

    RingBuffer(int size):ringSize(size),writeNeedle(0),contiguousWriteNeedle(0){
        buf.setSize(1, size);

    }
    AudioSampleBuffer buf;

    uint32 ringSize;
    uint32 writeNeedle;

    void writeBlock(AudioSampleBuffer & newBuf){
        int numChans =newBuf.getNumChannels();
        buf.setSize(numChans,buf.getNumSamples());
        int toCopy = newBuf.getNumSamples();

        if( writeNeedle + toCopy > ringSize){
            int firstSeg = ringSize-writeNeedle ;
            for(int i = numChans-1;i>=0 ;--i){
                buf.copyFrom(i, writeNeedle, newBuf, i, 0, firstSeg);
            }
            for(int i = numChans-1;i>=0 ;--i){
                buf.copyFrom(i,0,newBuf,i,firstSeg,toCopy-firstSeg);
            }
        }
        else{
            for(int i = numChans-1;i>=0 ;--i){
                buf.copyFrom(i, writeNeedle, newBuf,i, 0, toCopy);            }
        }
        writeNeedle+=toCopy;
        writeNeedle%=ringSize;

        // avoid wrapping errors when checking if contiguous need update
        contiguousWriteNeedle+=1;
    }


    const float* getLastBlock(int num,int channel = 0){

        if(num!=contiguousBuffer.getNumSamples() || writeNeedle!=contiguousWriteNeedle){
            updateContiguousBuffer(num);
        }
        else{
            DBG("alreadyComputed");
        }
        return contiguousBuffer.getReadPointer(channel);
    }


private:

    void updateContiguousBuffer(int num){

        jassert(num < (int)ringSize);
        contiguousBuffer.setSize(buf.getNumChannels(),num);
        int startIdx = writeNeedle-num;
        if(startIdx>=0){
            for(int i = buf.getNumChannels()-1;i>=0 ;--i){
                contiguousBuffer.copyFrom(i, 0, buf, i, startIdx, num);
            }
        }
        else{

            for(int i = buf.getNumChannels()-1;i>=0 ;--i){
                contiguousBuffer.copyFrom(i, 0, buf, i, ringSize+startIdx, -startIdx);
            }
            for(int i = buf.getNumChannels()-1;i>=0 ;--i){
                contiguousBuffer.copyFrom(i,-startIdx,buf,i,0,num+startIdx);
            }
        }
        contiguousWriteNeedle = writeNeedle;
    }
    AudioSampleBuffer contiguousBuffer;
    uint32 contiguousWriteNeedle = 0 ;
};






#endif  // AUDIOHELPERS_H_INCLUDED
