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
  BipBuffer(int _channels,int size):numChannels(_channels){
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
    jassert(num<=phantomSize);
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

class BipBufferTest  : public UnitTest
{
public:
  BipBufferTest() : UnitTest ("Ring buffer Test") {}



  void runTest() override
  {
    beginTest ("RingBufferTest");


    int numBlocks = 2;
    int blockSize = 512;
    int numChannels = 2;

    int ringSize = numBlocks*blockSize;
    int copiedSize = (numBlocks+2)*(blockSize);


    BipBuffer ring(numChannels,ringSize);

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


    // reading back
    for(int i = 0 ; i <copiedSize ;i+=blockSize){
      AudioBuffer<float>  b = ring.getLastBlock(blockSize);
      for(int c = 0  ;c < numChannels ; c++){
        for( int j = 0 ; j < blockSize ; j++){
        float fexpected =  expected.getSample(c, i*blockSize+j);
        float found = b.getSample(c,j);
          String err = "failed at :"+String(i)+ ",expect: "+String(fexpected)+"found: "+String(found) ;
          err+= " content of buffer in channel "+String(c)+" : \n";
          for( int k = 0 ; k < b.getNumSamples();k++){
            err+="\n "+String(k)+" : "+String (b.getSample(c,k));
          }
        expect(found == fexpected ,err);

        }
      }
    }
  }
};

static BipBufferTest ringBufferTest;

#endif


#endif  // RINGBUFFER_H_INCLUDED
