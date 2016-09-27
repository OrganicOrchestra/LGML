/*
 ==============================================================================

 LooperTest.cpp
 Created: 27 Sep 2016 6:58:42pm
 Author:  Martin Hermant

 ==============================================================================
 */
#include  "JuceHeader.h"

#if LGML_UNIT_TESTS
#include "LooperNode.h"
#include "TimeManager.h"

inline String BufToString(AudioBuffer<float> & b){String res;for(int i = 0 ; i < b.getNumSamples();i++){res+=String(b.getSample(0,i))+",";}return res;}
class LooperTest:public UnitTest{
public:
  LooperTest():UnitTest("LooperTest"){

  }
  LooperNode * currentLooper;
  AudioBuffer<float> testBuffer;
  
  const int recordSizeInBlock = 1000;

  void processBlock(){
     // it should ideally be independent of order between TimeManager and LooperProcessBlock
    static bool isBefore = true;// Random().nextBool();
    if(isBefore) TimeManager::getInstance()->incrementClock(getBlockSize());
    fillBufferWithTime(testBuffer);
    expect(getBlockSize() == testBuffer.getNumSamples(),"wrong buffer size ");
    MidiBuffer dumyMidi;
    currentLooper->processBlock(testBuffer,dumyMidi );


    if(!isBefore)TimeManager::getInstance()->incrementClock(getBlockSize());
  }

  void fillBufferWithRamp(AudioBuffer<float> & b){
    for(int i = 0 ; i < b.getNumSamples() ; i++){
      for(int j = 0 ; j  < b.getNumChannels() ; j ++){
        testBuffer.setSample(j, i, i);
      }
    }
  }
  float getBufferSampleForTime(uint64 t){
    static uint64 loopLength = getRecordedLength();
    return (t%loopLength);
  }
  void fillBufferWithTime(AudioBuffer<float> & b){

    
    for(int i = 0 ; i < b.getNumSamples() ; i++){
      float s = getBufferSampleForTime(TimeManager::getInstance()->timeState.time+i);
      for(int j = 0 ; j  < b.getNumChannels() ; j ++){
        b.setSample(j, i, s);
      }

    }
  }

  bool checkBufferAlignedForTime(AudioBuffer<float> & b,uint64 startTime){
    // byPass fade
    bool res = true;
    for(int i = 80 ; i < b.getNumSamples()-80 ; i++){
      float expected = getBufferSampleForTime(startTime+i);
      for(int j = 0 ; j  < b.getNumChannels() ; j ++){
        float actual = b.getSample(j, i);
        res&=(actual==expected);
      }
    }
    return  res;

  }
  int getBlockSize(){return currentLooper->getBlockSize();}
  int getRecordedLength(){return getBlockSize()*recordSizeInBlock;}


  void runTest()override{

    currentLooper  = new LooperNode();
    currentLooper->isMonitoring->setValue(false);
    currentLooper->quantization->setValue(-1);
    const int blockSize = currentLooper->getBlockSize();
    const int numChannels = currentLooper->getTotalNumOutputChannels();

    jassert(blockSize>0 );
    jassert(numChannels>0 );
    testBuffer.setSize(numChannels, blockSize);
    TimeManager * tm= TimeManager::getInstance();

    beginTest("basic playBackTest");
    expect(30*recordSizeInBlock*blockSize < std::numeric_limits<int>::max(),"integer not enough on this platform");
    LooperTrack * track1 = currentLooper->trackGroup.tracks[0];
    track1->recPlayTrig->trigger();
    for(int i = 0 ;i < recordSizeInBlock ; i++){
      processBlock();
      DBG(track1->loopSample.recordNeedle);
      float magnitude = testBuffer.getMagnitude(0,testBuffer.getNumSamples());
      expect(magnitude==0,"still monitoring");
    }
    track1->recPlayTrig->trigger();


    for(int i = 0 ;i < 2.3*recordSizeInBlock ; i++){
      processBlock();
      int offsetTime = 0;
      int wallTime =tm->getTimeInSample()%(recordSizeInBlock*blockSize);
      if(i==0){
        offsetTime = (tm->getTimeInSample())%(recordSizeInBlock*blockSize);
        wallTime = offsetTime;
      }
      float magnitude = testBuffer.getMagnitude(0,testBuffer.getNumSamples());
      expect(magnitude>0,"not Playing");
      int localTime =(i*blockSize+offsetTime)%(recordSizeInBlock*blockSize);

      expect(localTime==wallTime,"localTime not aligned : "+String(localTime)+","+String(wallTime));
      expect(checkBufferAlignedForTime(testBuffer,localTime),"buffer not aligned with time : "+String(wallTime));

    }


    int recLen =track1->loopSample.getRecordedLength() ;
    int offset = recLen%(tm->beatTimeInSample);
    expect(recLen== getRecordedLength(),"wrong recorded Length");
    expect(offset==0);
    float bL1 = track1->beatLength->floatValue();
    expect(bL1==(int)bL1,"beatLength not an integer : "+String(bL1));
    double sB1=track1->startPlayBeat;
    expect(sB1 == (int)sB1 , "startBeat not integer : "+String(sB1));

    /////////////////////
    ///
    beginTest("2 track Sync");
    LooperTrack * track2 = currentLooper->trackGroup.tracks[1];

    track2->recPlayTrig->trigger();
    uint64 startRec = tm->getNextGlobalQuantifiedTime();
    float secondRecordedLengthFloat = 3.3*recordSizeInBlock;



    for(int i = 0 ;i < secondRecordedLengthFloat ; i++){
      processBlock();
      float magnitude = testBuffer.getMagnitude(0,testBuffer.getNumSamples());
      expect(magnitude>0,"not Playing");

    }
    track2->recPlayTrig->trigger();
    uint64 endRec = tm->getNextGlobalQuantifiedTime();
    while(tm->timeState.time<= endRec){
      processBlock();
    }
    expect(track2->loopSample.isPlaying(),"not ended recording");
    recLen =track2->loopSample.getRecordedLength() ;
    int recDiff = recLen - (endRec - startRec);
    offset = recLen%(tm->beatTimeInSample);
    expect(recDiff == 0,"wrong recorded Length");
    expect(offset==0,"unaligned recorded length");
    float bL2 = track2->beatLength->floatValue();
    expect(bL2==(int)bL2,"beatLength not an integer : "+String(bL2));
    double sB2=track2->startPlayBeat;
    expect(sB2 == (int)sB2 , "startBeat not integer : "+String(sB2));

    beginTest("check sampleAccurate align");
    int maxCommonSamples = jmax(track2->loopSample.getRecordedLength(),track1->loopSample.getRecordedLength());
    for(int i = 80 ; i < maxCommonSamples-80;i++){
      int j1 = i%getRecordedLength();
      double beatsPerRecord = getRecordedLength()/tm->beatTimeInSample;
      int j2 = (int)(i + floor(sB2/beatsPerRecord)*beatsPerRecord*tm->beatTimeInSample)%(track2->loopSample.getRecordedLength());
      for(int c  = 0 ; c < numChannels ; c++){
        float normal1 = getBufferSampleForTime(j1);
        float normal2 = getBufferSampleForTime(j2);
        float track1Val =(int)(track1->loopSample.loopSample.getSample(c, j1));
        float track2Val =(int)(track2->loopSample.loopSample.getSample(c, j2));
        expect(track1Val==track2Val,"loops not aligned at: "+String (i)+" :: " + String(j1) + ","+String(j2));
      }
    }
    
    
    
  }
  
};


static LooperTest looperTest;







#endif // unitTest
