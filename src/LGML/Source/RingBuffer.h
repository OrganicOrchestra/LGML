/*
 ==============================================================================

 RingBuffer.h
 Created: 6 Jun 2016 7:45:00pm
 Author:  Martin Hermant

 ==============================================================================
 */

#ifndef RINGBUFFER_H_INCLUDED
#define RINGBUFFER_H_INCLUDED




/*
 helper class for bipartite buffer
 allowing having constant access to contiguous memory in a circular buffer
 */
class BipBuffer{

public:
  BipBuffer(int _channels,int size):numChannels(_channels){
    phantomSize = ceil(size*2.0/3.0);
    buf.setSize(_channels,3*phantomSize,false,true);
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
      writeNeedle+=firstSeg;
      writeNeedle%=2*phantomSize;
      for(int i = numChannels-1;i>=0 ;--i){
        safeCopy(newBuf.getReadPointer(i,firstSeg),toCopy-firstSeg,i);
      }
      writeNeedle+=toCopy-firstSeg;
      writeNeedle%=2*phantomSize;
    }
    else{
      for(int i = numChannels-1;i>=0 ;--i){
        safeCopy(newBuf.getReadPointer(i),toCopy,i);
      }
      writeNeedle+=toCopy;
      writeNeedle%=2*phantomSize;
    }
  }


  const AudioBuffer<float> & getLastBlock(int num){
    jassert(num<=buf.getNumSamples()/2);
    pointers.ensureStorageAllocated(numChannels);
    for(int i = 0 ; i < numChannels ; i++){
      pointers.set(i,buf.getArrayOfWritePointers()[i] + phantomSize+ writeNeedle-num);
    }
    contiguousBuffer.setDataToReferTo(pointers.getRawDataPointer(), numChannels, num);
    return contiguousBuffer;
  }

  const void printContent(){
    String niceOutput;
    for(int i = 0 ; i < buf.getNumSamples() ; i++){
      for(int c = 0 ;c < numChannels ; c++){niceOutput+="\t  "+String(buf.getSample(c,i));}
      if(i== phantomSize+ writeNeedle){niceOutput+="\t  writeNeedle";}
      if(i%phantomSize==0){niceOutput+="\t  phantomLimit";}
      niceOutput+="\n";
    }
    DBG(niceOutput);
  }
  


  AudioSampleBuffer buf;
private:
  void safeCopy(const float * b,int numSample,int channel){

    // overLapping
    if(writeNeedle < phantomSize && writeNeedle+numSample>phantomSize){
      int overflow = writeNeedle+numSample - phantomSize;
      int left = numSample-overflow;
      buf.copyFrom(channel, phantomSize+writeNeedle,b,left);
      buf.copyFrom(channel, phantomSize+writeNeedle+left,b+left,overflow);
      buf.copyFrom(channel, writeNeedle+left-phantomSize,b+left,overflow);
    }
    // second phantomZone
    else if (writeNeedle+numSample>phantomSize){
      buf.copyFrom(channel, phantomSize+writeNeedle,b,numSample);
      buf.copyFrom(channel, writeNeedle-phantomSize,b,numSample);
    }
    // first phantomZone
    else{
      buf.copyFrom(channel, phantomSize+writeNeedle, b, numSample);
    }


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
  BipBufferTest() : UnitTest ("RingBufferTest") {}



  void runTest() override
  {
    beginTest ("readBack");


    int numBlocks = 2;
    int blockSize = 10;
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
        expect(tstBuf.getMagnitude(0, tstBuf.getNumSamples())>0, "setting empty buffer");
        ring.writeBlock(tstBuf);
      }

      if(i>=(copiedSize - ringSize)){
        for (int c = 0 ;  c < numChannels ; c++){
          expected.setSample(c, i-(copiedSize - ringSize), dstValue);
        }
      }


    }


    // reading last
    int blockNum = ceil(ringSize/blockSize);
      const AudioBuffer<float>  b = ring.getLastBlock(blockNum*blockSize);
      for(int c = 0  ;c < numChannels ; c++){
        for( int j = 0 ; j < blockNum*blockSize ; j++){
          float fexpected =  expected.getSample(c, j);
          float found = b.getSample(c,j);
          String err = "failed at :"+String(j)+ ",expect: "+String(fexpected)+"found: "+String(found) ;
          expect(found == fexpected ,err);
          
        }
      }

  }
};

static BipBufferTest ringBufferTest;

#endif


#endif  // RINGBUFFER_H_INCLUDED
