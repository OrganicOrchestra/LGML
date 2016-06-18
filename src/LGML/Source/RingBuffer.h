/*
  ==============================================================================

    RingBuffer.h
    Created: 6 Jun 2016 7:45:00pm
    Author:  Martin Hermant

  ==============================================================================
*/

#ifndef RINGBUFFER_H_INCLUDED
#define RINGBUFFER_H_INCLUDED

#include "JuceHeader.h"



/*
 helper class for bipartite buffer
 allowing having constant access to contiguous memory in a circular buffer
 */
class BipBuffer{

public:
    BipBuffer(int _channels,int size,int maxBlockSize):numChannels(_channels){
        phantomSize = size;
        buf.setSize(_channels,3*size,false,true);
        writeNeedle = 0;
    }

    void writeBlock(AudioSampleBuffer & newBuf){


        jassert(newBuf.getNumChannels()>=numChannels);
        int toCopy = newBuf.getNumSamples();

        if( writeNeedle + toCopy > 2*phantomSize){
            int firstSeg = 2*phantomSize-(writeNeedle) ;
            jassert(firstSeg<newBuf.getNumSamples());
            for(int i = numChannels-1;i>=0 ;--i){
                safeCopy(newBuf.getReadPointer(i,0),firstSeg,i);
            }
            for(int i = numChannels-1;i>=0 ;--i){
                safeCopy(newBuf.getReadPointer(i,firstSeg),toCopy-firstSeg,i);
            }
        }
        else{
            for(int i = numChannels-1;i>=0 ;--i){
                safeCopy(newBuf.getReadPointer(i),toCopy,i);
            }
        }
    }


    const AudioBuffer<float> & getLastBlock(int num){
        jassert(num<phantomSize);
        pointers.ensureStorageAllocated(numChannels);
        for(int i = 0 ; i < numChannels ; i++){
            pointers.set(i,buf.getArrayOfWritePointers()[i] + phantomSize+ writeNeedle-num);
            DBG((uint64)pointers[i]);
        }
        contiguousBuffer.setDataToReferTo(pointers.getRawDataPointer(), numChannels, num);
        return contiguousBuffer;
    }

    AudioSampleBuffer buf;
private:
    void safeCopy(const float * b,int s,int channel){
        buf.copyFrom(channel, phantomSize+writeNeedle, b, s);

        if(writeNeedle>phantomSize){
            buf.copyFrom(channel, writeNeedle,b,s);
        }
        writeNeedle+=s;
        writeNeedle%=2*phantomSize;

    }
    int writeNeedle;
    int phantomSize;
    int numChannels ;
    AudioBuffer<float> contiguousBuffer;
    Array<float*> pointers ;
    
    
    
    
};




//==============================================================================
//==============================================================================
#if LGML_UNIT_TESTS

class RingBufferTest  : public UnitTest
{
public:
    RingBufferTest() : UnitTest ("Ring buffer Test") {}



    void runTest() override
    {
        beginTest ("RingBufferTest");


        int numBlocks = 2;
        int blockSize = 512;
        int numChannels = 2;

        int ringSize = numBlocks*blockSize;
        int copiedSize = (numBlocks+2)*(blockSize);


        RingBuffer ring(ringSize);

        AudioBuffer<float> tstBuf;
        tstBuf.setSize(numChannels,blockSize);

        AudioBuffer<float> expected;
        expected.setSize(numChannels,ringSize);


        for (int i = 0 ; i < copiedSize ; i++){

            float dstValue = i;
            for (int c = 0 ;  c < numChannels ; c++){
                tstBuf.setSample(c, i%blockSize, dstValue);

            }
            if((i+1)%blockSize ==0){
                ring.writeBlock(tstBuf);
            }

            if(i-(copiedSize - ringSize)>=0){
                for (int c = 0 ;  c < numChannels ; c++){
                    expected.setSample(c, i-(copiedSize - ringSize), dstValue);
                }
            }


        }


        for(int c = 0  ;c < numChannels ; c++){
            const float * ptr = ring.getLastBlock(ringSize,c);
            for(int i = 0 ; i < expected.getNumSamples() ;i++){
                float fexpected =  expected.getSample(c, i);
                float found = ptr[i];
                expect(found == fexpected ,
                       "failed at :"+String(i)+ ",expect: "+String(fexpected)+"found: "+String(found) );


            }
        }
    }
};

static RingBufferTest ringBufferTest;

#endif





//not used atm
// triggers glith in some time first time buffer is allocated...

//
//class RingBuffer{
//public:
//
//    RingBuffer(int _channels,int size,int maxBlockSize):channels(_channels),ringSize(size),writeNeedle(0),contiguousWriteNeedle(0){
//        buf.setSize(channels, size);
//        buf.clear();
//        contiguousBuffer.setSize(channels, maxBlockSize);
//        contiguousOutDated = true;
//
//    }
//    AudioSampleBuffer buf;
//    bool contiguousOutDated;
//
//    uint32 ringSize;
//    uint32 writeNeedle;
//
//    void writeBlock(AudioSampleBuffer & newBuf){
//        int numChans =channels;
//        int toCopy = newBuf.getNumSamples();
//        buf.setSize(numChans,buf.getNumSamples(),true,true,true);
//
//
//        if( writeNeedle + toCopy > ringSize){
//            int firstSeg = ringSize-writeNeedle ;
//            for(int i = numChans-1;i>=0 ;--i){
//                buf.copyFrom(i, writeNeedle, newBuf, i, 0, firstSeg);
//            }
//            for(int i = numChans-1;i>=0 ;--i){
//                buf.copyFrom(i,0,newBuf,i,firstSeg,toCopy-firstSeg);
//            }
//        }
//        else{
//            for(int i = numChans-1;i>=0 ;--i){
//                buf.copyFrom(i, writeNeedle, newBuf,i, 0, toCopy);            }
//        }
//        writeNeedle+=toCopy;
//        writeNeedle%=ringSize;
//
//        // avoid wrapping errors when checking if contiguous need update
//        contiguousOutDated = true;
//    }
//
//
//    const AudioBuffer<float> & getLastBlock(int num){
//
//        if(num!=contiguousBuffer.getNumSamples() || contiguousOutDated){
//            updateContiguousBuffer(num);
//        }
//
//        return contiguousBuffer;
//    }
//
//
//private:
//
//    void updateContiguousBuffer(int num){
//
//        jassert(num <= (int)ringSize);
//        contiguousBuffer.setSize(buf.getNumChannels(),num,false,false,true);
//        int startIdx = writeNeedle-num;
//        if(startIdx>=0){
//            for(int i = buf.getNumChannels()-1;i>=0 ;--i){
//                contiguousBuffer.copyFrom(i, 0, buf, i, startIdx, num);
//            }
//        }
//        else{
//
//            for(int i = buf.getNumChannels()-1;i>=0 ;--i){
//                contiguousBuffer.copyFrom(i, 0, buf, i, ringSize+startIdx, -startIdx);
//            }
//            for(int i = buf.getNumChannels()-1;i>=0 ;--i){
//                contiguousBuffer.copyFrom(i,-startIdx,buf,i,0,num+startIdx);
//            }
//        }
//        contiguousWriteNeedle = writeNeedle;
//        contiguousOutDated = false;
//    }
//    AudioSampleBuffer contiguousBuffer;
//    uint32 contiguousWriteNeedle = 0 ;
//    int channels;
//};


#endif  // RINGBUFFER_H_INCLUDED
