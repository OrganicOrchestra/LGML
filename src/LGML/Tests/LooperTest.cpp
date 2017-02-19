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

inline String BufToString(AudioBuffer<float> & b,int start = 0 ,int numSamples =-1){
  String res;if(numSamples==-1)numSamples = b.getNumSamples();
  for(int i = start ; i < numSamples;i++){res+=String(b.getSample(0,i))+",";}
  return res;}

class LooperTest:public UnitTest{
public:
  LooperTest():UnitTest("LooperTest"){

  }
  ScopedPointer<LooperNode> currentLooper;
  AudioBuffer<float> testBuffer;

  int blockSize = 256;
  int sampleRate = 44100;
  int originTargetBeatSize = 0.6*sampleRate;
  const int numBeats = 4;
  const int recordSizeInBlock = floor(numBeats*originTargetBeatSize*1.0/blockSize);
  int fadeSample = 80;

  void processBlock(){
    // TimeManager is the first call back in DSP cycle
    TimeManager::getInstance()->incrementClock(getBlockSize());
    fillBufferWithTime(testBuffer);
    expect(getBlockSize() == testBuffer.getNumSamples(),"wrong buffer size ");
    static MidiBuffer dumyMidi;
    currentLooper->processBlock(testBuffer,dumyMidi );
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
    for(int i = fadeSample ; i < b.getNumSamples()-fadeSample ; i++){
      float expected = getBufferSampleForTime(startTime+i);
      for(int j = 0 ; j  < b.getNumChannels() ; j ++){
        float actual = b.getSample(j, i);
        if( j==0 && actual!=expected)DBG(actual<<","<< expected);
        res&=(actual==expected);
      }
    }
    return  res;

  }


  int getBlockSize(){return currentLooper->getBlockSize();}

  int getRecordedLength(){uint64 expected  =  getBlockSize()*recordSizeInBlock;
    expected-= expected%TimeManager::getInstance()->samplePerBeatGranularity;
    return expected;
  }


  void runTest()override{

    currentLooper  = new LooperNode();
    TimeManager * tm= TimeManager::getInstance();
    currentLooper->setPlayConfigDetails(2,2, sampleRate, blockSize);
    currentLooper->isMonitoring->setValue(false);
    currentLooper->quantization->setValue(-1);
    const int blockSize = currentLooper->getBlockSize();
    const int numChannels = currentLooper->getTotalNumOutputChannels();

    int targetBeatSize = recordSizeInBlock*blockSize/numBeats;

    beginTest("basic playBackTest");
    expect((originTargetBeatSize>tm->beatTimeGuessRange.getStart()*sampleRate )&& (originTargetBeatSize<tm->beatTimeGuessRange.getEnd()*sampleRate),"wrong choice of tempo");

    jassert(blockSize>0 );
    jassert(numChannels>0 );
    testBuffer.setSize(numChannels, blockSize);



    expect(30*recordSizeInBlock*blockSize < std::numeric_limits<int>::max(),"integer not enough on this platform");
    expect(blockSize ==getBlockSize(),"looper has the wrong blockSize");
    LooperTrack * track1 = currentLooper->trackGroup.tracks[0];
    fadeSample = track1->playableBuffer.getNumSampleFadeOut();
    track1->recPlayTrig->trigger();
    for(int i = 0 ;i < recordSizeInBlock ; i++){
      processBlock();
      float magnitude = testBuffer.getMagnitude(0,testBuffer.getNumSamples());
      expect(magnitude==0,"still monitoring : "+String(magnitude));
    }
    expect(track1->playableBuffer.recordNeedle == recordSizeInBlock*blockSize,"jumped a block while recording : "+String(track1->playableBuffer.recordNeedle)+" : "+String(recordSizeInBlock*blockSize));
    track1->recPlayTrig->trigger();


    for(int i = 0 ;i < 2.3*recordSizeInBlock ; i++){
      processBlock();


      int startWallTime =tm->getTimeInSample()%(recordSizeInBlock*blockSize);
      int endWallTime = (tm->getNextTimeInSample())%(recordSizeInBlock*blockSize);
      expect(endWallTime!=startWallTime,"block unused");
      int offset = 0;
      // need to be first playedSample
      if(i==0){
        expect(track1->playableBuffer.isPlaying(),"not Playing");
        offset =(recordSizeInBlock*blockSize)-tm->getTimeInSample();
        startWallTime=(startWallTime+offset)%(recordSizeInBlock*blockSize);
        int recLen =track1->playableBuffer.getRecordedLength() ;
        int offset = recLen%(tm->beatTimeInSample);
        expect(recLen== getRecordedLength(),"wrong recorded Length found : "+String(recLen)+" for "+String( getRecordedLength()));
        expect(offset==0,"wrong quantization");
      }

      expect(track1->playableBuffer.playNeedle==(endWallTime%track1->playableBuffer.recordNeedle),"unaligned PlayNeedle : "+String(track1->playableBuffer.playNeedle)+" , "+String(endWallTime%track1->playableBuffer.recordNeedle) );

      float magnitude = testBuffer.getMagnitude(0,testBuffer.getNumSamples());
      expect(magnitude>0,"not Playing");
      int localTime =(i*blockSize)%(recordSizeInBlock*blockSize);

      expect(localTime==startWallTime,"localTime not aligned : "+String(localTime)+","+String(startWallTime));
      expect(checkBufferAlignedForTime(testBuffer,startWallTime),"buffer not aligned with time : "+String(startWallTime));

    }
    expect(targetBeatSize ==tm->beatTimeInSample,"targetBeatSize not reached : "+String(targetBeatSize-(int)tm->beatTimeInSample));


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
    expect(track2->playableBuffer.isPlaying(),"not ended recording");

    uint64 recLen =track2->playableBuffer.getRecordedLength() ;
    int recDiff = recLen - (endRec - startRec);
    uint offset = recLen%(tm->beatTimeInSample);
    expect(recDiff == 0,"wrong recorded Length");
    expect(offset==0,"unaligned recorded length");
    float bL2 = track2->beatLength->floatValue();
    expect(bL2==(int)bL2,"beatLength not an integer : "+String(bL2));
    double sB2=track2->startPlayBeat;
    expect(sB2 == (int)sB2 , "startBeat not integer : "+String(sB2));
    expect(endRec ==sB2*tm->beatTimeInSample,"wrong start beat");
    beginTest("check sampleAccurate align");
    int minCommonSamples = jmin(track2->playableBuffer.getRecordedLength(),track1->playableBuffer.getRecordedLength());
    int maxCommonSamples = jmax(track2->playableBuffer.getRecordedLength(),track1->playableBuffer.getRecordedLength());
    for(int i = fadeSample  ; i <  maxCommonSamples-fadeSample;i++){
      int j1 = i%getRecordedLength();
//      double beatsPerRecord = getRecordedLength()/tm->beatTimeInSample;
      int j2 = (int)(i )%(track2->playableBuffer.getRecordedLength());
      if(j1>fadeSample && getRecordedLength()-j1 > fadeSample && j2>fadeSample && track2->playableBuffer.getRecordedLength() - j2>fadeSample){
        for(int c  = 0 ; c < numChannels ; c++){
          float normal1 = getBufferSampleForTime(j1);
          float normal2 = getBufferSampleForTime(j2+sB2*tm->beatTimeInSample);
          float track1Val =(int)(track1->playableBuffer.audioBuffer.getSample(c, j1));
          float track2Val =(int)(track2->playableBuffer.audioBuffer.getSample(c, j2));
          expect(normal1==track1Val,"wrong alignement test on track 1");
          expect(normal2==track2Val,"wrong alignement test on track 2");
          if(i < minCommonSamples - fadeSample)expect(track1Val==track2Val,"loops not aligned at: "+String (i)+" :: " + String(j1) + ","+String(j2));
        }
      }
    }
    
    currentLooper = nullptr;
    
  }
  
};


static LooperTest looperTest;







#endif // unitTest
