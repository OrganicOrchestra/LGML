/*
  ==============================================================================

    BufferBlockList.cpp
    Created: 15 Jul 2017 3:05:20pm
    Author:  Martin Hermant

  ==============================================================================
*/

#include "BufferBlockList.h"




void BufferBlockList::allocateSamples(int numChannels,int numSamples){
  if(size()>0 && getUnchecked(0).getNumChannels()!=numChannels){
    for(auto & b:*this){
      b.setSize(numChannels, BUFFER_BLOCK_SIZE);
    }
  }
  int i = size() * BUFFER_BLOCK_SIZE;

  while (i < numSamples){
    add(AudioSampleBuffer(BUFFER_BLOCK_SIZE,numChannels));
    i+=BUFFER_BLOCK_SIZE;
  }

  while(i>numSamples+BUFFER_BLOCK_SIZE){
    removeLast();
    i-=BUFFER_BLOCK_SIZE;

  }



}

void BufferBlockList::setNumChannels(int numChannels){
  for(auto & b:*this){
    b.setSize(numChannels, BUFFER_BLOCK_SIZE);
  }
}
void BufferBlockList::setNumSample(int numSamples){
  allocateSamples(getAllocatedNumChannels(), numSamples);
  targetNumSamples = numSamples;
}
int BufferBlockList::getNumSamples() const{
  return targetNumSamples;
}
int BufferBlockList::getAllocatedNumSample()const {
  return size()*BUFFER_BLOCK_SIZE;
}
int BufferBlockList::getAllocatedNumChannels()const{
  return size()>0?getUnchecked(0).getNumChannels():0;
}
void BufferBlockList::copyTo(AudioSampleBuffer & outBuf,int sampleStart,int numSampleToCopy){
  if(numSampleToCopy==-1) numSampleToCopy = outBuf.getNumSamples();
  jassert(numSampleToCopy <= size()*BUFFER_BLOCK_SIZE);
  int numChannels = jmin(getAllocatedNumChannels(),outBuf.getNumChannels());
  int sampleProcessed = 0;
  int readPos = sampleStart;
  int writeBlockIdx;
  int writePos = 0;

  while(sampleProcessed < numSampleToCopy){
    writeBlockIdx=floor(readPos*1.0/BUFFER_BLOCK_SIZE);
    int startWrite = readPos%BUFFER_BLOCK_SIZE ;
    int endWrite =  writePos+ BUFFER_BLOCK_SIZE>numSampleToCopy? numSampleToCopy-writePos: BUFFER_BLOCK_SIZE;
    int blockSize = endWrite - startWrite;
    for (int i = 0 ; i < numChannels ; i++){
      outBuf.copyFrom( i, sampleProcessed, getUnchecked(writeBlockIdx),i, startWrite, blockSize);
    }
    readPos+=blockSize;
    writePos+=blockSize;
    sampleProcessed+=blockSize;

    if(readPos>=targetNumSamples){
      jassert(readPos==targetNumSamples);
      readPos=0;
      writeBlockIdx = 0;
    }

    

  }

}

void BufferBlockList::copyFrom(const AudioSampleBuffer & inBuf,int sampleStart,int numSampleToCopy){
  jassert(sampleStart< getAllocatedNumSample());
  if (numSampleToCopy==-1)numSampleToCopy =  inBuf.getNumSamples();
  jassert(inBuf.getNumChannels()==getAllocatedNumChannels());
  int numChannels = inBuf.getNumChannels();
  int sampleProcessed = 0;
  int readPos = sampleStart;

  int writeBlockIdx ;
  int writePos = 0;

  while(sampleProcessed < numSampleToCopy){
    writeBlockIdx=floor(readPos*1.0/BUFFER_BLOCK_SIZE);
    int startWrite = readPos%BUFFER_BLOCK_SIZE ;
    int endWrite =   writePos + BUFFER_BLOCK_SIZE>numSampleToCopy? numSampleToCopy-writePos + startWrite: BUFFER_BLOCK_SIZE;
    int blockSize = endWrite - startWrite;
    for (int i = 0 ; i < numChannels ; i++){
      getUnchecked(writeBlockIdx).copyFrom(i, startWrite, inBuf, i, writePos, blockSize);
    }
    readPos+=blockSize;
    writePos+=blockSize;
    sampleProcessed+=blockSize;

    jassert(readPos<getAllocatedNumSample());

  }
}


AudioSampleBuffer & BufferBlockList::getContiguousBuffer(int sampleStart,int numSamples){
  if(numSamples==-1)numSamples = getNumSamples()-sampleStart;
  contiguous_Cache.setSize(getAllocatedNumChannels(), numSamples);
  copyTo(contiguous_Cache,sampleStart,numSamples);
  return contiguous_Cache;
}
