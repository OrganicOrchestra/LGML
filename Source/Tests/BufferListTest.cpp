/*
  ==============================================================================

    BufferListTest.cpp
    Created: 18 Jul 2017 1:48:39pm
    Author:  Martin Hermant

  ==============================================================================
*/

#if LGML_UNIT_TESTS
#include "../Audio/BufferBlockList.h"


inline String BufToString(AudioBuffer<float> & b,int start = 0 ,int numSamples =-1){
  String res;if(numSamples==-1)numSamples = b.getNumSamples();
  for(int i = start ; i < numSamples;i++){res+=String(b.getSample(0,i))+",";}
  return res;}

class BufferBlockListTest:public UnitTest{
public:
  BufferBlockListTest():UnitTest("BufferBlockList"){

  }

  AudioBuffer<float> testBuffer;



  void fillBufferWithRamp(AudioBuffer<float> & b){
    for(int i = 0 ; i < b.getNumSamples() ; i++){
      for(int j = 0 ; j  < b.getNumChannels() ; j ++){
        testBuffer.setSample(j, i, i+1);
      }
    }
  }






  void runTest()override{
    {
    beginTest(" io on bufferblocklist ");
      BufferBlockList bl;
      testBuffer.setSize(2, 950);
      bl.allocateSamples(2, 1000);
    fillBufferWithRamp(testBuffer);
      bl.copyFrom(testBuffer,0);
      for(int i = 0 ; i < testBuffer.getNumSamples() ; i++){
        for(int j = 0 ; j  < testBuffer.getNumChannels() ; j ++){
          expect(testBuffer.getSample(j, i) == bl.getSample(j,i));
        }
      }
    }

  }
  
};


static BufferBlockListTest bufferBlockListTest;







#endif // unitTest
