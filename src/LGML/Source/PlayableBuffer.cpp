
/*
 ==============================================================================

 PlayableBuffer.cpp
 Created: 10 Jan 2017 4:40:14pm
 Author:  Martin Hermant

 ==============================================================================
 */

#include "PlayableBuffer.h"



#include "AudioDebugPipe.h"


extern ThreadPool * getEngineThreadPool();


#if BUFFER_CAN_STRETCH
#include "StretcherJob.h"
#include "rubberband/RubberBandStretcher.h"
using namespace RubberBand;
#endif



//////////////////////
// Playable buffer


PlayableBuffer::PlayableBuffer(int numChannels,int numSamples,float _sampleRate,int _blockSize):

recordNeedle(0),

playNeedle(0),globalPlayNeedle(0),
state(BUFFER_STOPPED),
lastState(BUFFER_STOPPED),
stateChanged(false),
numTimePlayed(0),
sampleOffsetBeforeNewState(0),
multiNeedle(512,512)
//tailRecordNeedle(0),

#if BUFFER_CAN_STRETCH
,stretchJob(nullptr)
#if RT_STRETCH
,pendingTimeStretchRatio(1)
,isStretchPending(false)
,isStretchReady(false)
#endif
#endif

,bufferBlockList(numChannels,numSamples)
,blockSize(_blockSize)
,sampleRate(_sampleRate)
,fadePendingStretch(256,256)
{

  jassert(numSamples < std::numeric_limits<int>::max());

#if RT_STRETCH
  fadePendingStretch.setFadedOut();
  initRTStretch();
#endif
}

PlayableBuffer::~PlayableBuffer(){

}


void PlayableBuffer::setNumChannels(int numChannels){
  bufferBlockList.setNumChannels(numChannels);
#if RT_STRETCH
  initRTStretch();
#endif

}
int PlayableBuffer::getNumChannels() const{
  return bufferBlockList.getAllocatedNumChannels();
}
int PlayableBuffer::getAllocatedNumSample() const{
  return bufferBlockList.getAllocatedNumSample();
}
void PlayableBuffer::setBlockSize(int bs){
  blockSize = bs;
#if RT_STRETCH
  initRTStretch();
#endif
}

bool PlayableBuffer::processNextBlock(AudioBuffer<float> & buffer,uint64 time){
  bool succeeded = true;
  if(buffer.getNumSamples()!=blockSize){
    setBlockSize(buffer.getNumSamples());
  }


  if(sampleOffsetBeforeNewState){
    jassert(isFirstRecordingFrame() || wasLastRecordingFrame() || globalPlayNeedle==0);
  }

  if (isFirstRecordingFrame()){
    succeeded &= writeAudioBlock(buffer, sampleOffsetBeforeNewState);

  }
  else if(isRecording() ){
    succeeded &= writeAudioBlock(buffer);
  }
  else if( wasLastRecordingFrame()){
    succeeded &= writeAudioBlock(buffer, 0,sampleOffsetBeforeNewState);
    //    findFadeLoopPoints();



  }

  //  if( isRecordingTail() ){
  //    succeeded &= writeAudioBlock(buffer,sampleOffsetBeforeNewState,-1,true);
  //  }

  // save to buffer when end recording
  if( wasLastRecordingFrame() && recordNeedle>0 && originAudioBuffer.getNumSamples()==0){
    originAudioBuffer.setSize(getNumChannels(), getRecordedLength());
    bufferBlockList.setNumSample(getRecordedLength());
    fadeInOut();
    bufferBlockList.copyTo(originAudioBuffer,0,0,getRecordedLength());

  }

  buffer.clear();


  //  if ( isOrWasPlaying()){




#if RT_STRETCH



  if(!processPendingRTStretch(buffer,time))
    readNextBlock(buffer,time,sampleOffsetBeforeNewState);
  //  int stretchedSample = tmpBufferStretch.getNumSamples();
  if(originAudioBuffer.getNumSamples()==0){
    isStretchPending =false;
    isStretchReady =false;
  }
  if(isStretchReady){
    if(fadePendingStretch.getLastFade()==0){
      if(isPlaying())setPlayNeedle ( time%tmpBufferStretch.getNumSamples());
      applyStretch();
    }
    else if (!fadePendingStretch.isFadingOut()){
      if(onsetSamples.size()>0){
        for(auto o:onsetSamples){
          int delta =(int)((long long)o-(long long)(stretchNeedle %originAudioBuffer.getNumSamples()) );
          if(abs(delta) <= fadePendingStretch.fadeOutNumSamples ){
            fadePendingStretch.startFadeOut();
            break;
          }
        }
      }
      else{
      fadePendingStretch.startFadeOut();
      }
    }





  }
#else
  readNextBlock(buffer,time,sampleOffsetBeforeNewState);
#endif

  //  }
  if(isStopped()&& playNeedle>0){
    multiNeedle.fadeAllOut();
    //     setPlayNeedle(0);
    playNeedle = 0;
    //    globalPlayNeedle = 0;
  }

  return succeeded;
}



bool PlayableBuffer::writeAudioBlock(const AudioBuffer<float> & buffer, int fromSample,int samplesToWrite){



  samplesToWrite= samplesToWrite==-1?buffer.getNumSamples()-fromSample:samplesToWrite;

  if(samplesToWrite==0){return true;}
  //  float rms = buffer.getRMSLevel(0, fromSample,samplesToWrite);
  //  if(rms==0){
  //    int dbg;
  //    dbg++;
  //  }


  if (recordNeedle + buffer.getNumSamples()> getAllocatedNumSample()) {
    bufferBlockList.allocateSamples(bufferBlockList.getAllocatedNumChannels(), recordNeedle + 10*buffer.getNumSamples());
    //    return true;
  }
  //  else{
  //    const int maxChannel = jmin(getNumChannels(),buffer.getNumChannels());
  bufferBlockList.copyFrom(buffer, recordNeedle);
  //    for (int i =  maxChannel- 1; i >= 0; --i) {
  //      audioBuffer.copyFrom(i, (int)recordNeedle, buffer, i, fromSample, samplesToWrite );
  //    }

  recordNeedle += samplesToWrite;
  //  }

  return true;
}


inline void PlayableBuffer::readNextBlock(AudioBuffer<float> & buffer,uint64 time,int fromSample   ){
  if(isRecording()){
    //    buffer.clear();
    //    jassertfalse;
    return;
  }

  if(fromSample>0){
    jassert(isFirstPlayingFrame() || (playNeedle-fromSample+getRecordedLength()) % getRecordedLength()==0);
    int dbg;
    dbg++;
  }
  int numSamples = buffer.getNumSamples()-fromSample;


  if(wasLastRecordingFrame()){
    multiNeedle.setLoopSize(getRecordedLength());
  }

  if(state==BUFFER_PLAYING && !isStretchPending){
    jassert(multiNeedle.loopSize == getRecordedLength() );
    int targetTime = (time  + getRecordedLength())%getRecordedLength();
    if(targetTime != playNeedle){
      //      jassertfalse;
      setPlayNeedle(targetTime);

    }
  }



  // assert false for now to check alignement
  if(isFirstPlayingFrame()){

#if !LINK_SUPPORT
    jassert(playNeedle==0);
#endif
  }


  //  buffer.clear();
  if(recordNeedle>0 && !isRecording() ){
    if(isPlaying() && multiNeedle.currentPos!=playNeedle && !isStretchPending){
      multiNeedle.jumpTo(playNeedle);
      jassertfalse;
    }

    multiNeedle.addToBuffer(bufferBlockList, buffer, buffer.getNumSamples(), isPlaying());
    //    }

  }
  if(isPlaying()){
    playNeedle += numSamples;
    globalPlayNeedle+=numSamples;
    if(playNeedle>=getRecordedLength()){
      numTimePlayed ++;
    }
    playNeedle %= getRecordedLength();
  }
  else{
    int dbg;dbg++;
  }


}


void PlayableBuffer::setPlayNeedle(int n){
  if(n<0){
    n=getRecordedLength()+n;
  }
  multiNeedle.jumpTo(n);
  playNeedle = n;

}

void PlayableBuffer::cropEndOfRecording(int * sampletoRemove){
  if(*sampletoRemove>recordNeedle){
    *sampletoRemove = 0;
  }

  recordNeedle-=*sampletoRemove;
  multiNeedle.setLoopSize(recordNeedle);
}
//void PlayableBuffer::padEndOfRecording(int sampleToAdd){
//  audioBufferList.clear((int)recordNeedle, sampleToAdd);
//  recordNeedle+=sampleToAdd;
//  multiNeedle.setLoopSize(recordNeedle);
//}
void PlayableBuffer::setRecordedLength(uint64 targetSamples){
  //  jassert(targetSamples<=getAllocatedNumSample());
  recordNeedle = targetSamples;
  multiNeedle.setLoopSize(targetSamples);
  bufferBlockList.setNumSample(targetSamples);
  //  findFadeLoopPoints();

}
//
//inline int findFirstZeroCrossing(const AudioBuffer<float> & b, int start,int end,int c){
//  float fS = b.getSample(c, start);
//  if(fS ==0) return start;
//  bool sgn = fS>0;
//  if( start>end){
//    for(int i = start-1 ; i > end ; i--){
//      if((b.getSample(c,i)>0)!=sgn){ return i;}
//    }
//  }
//  else{
//    for(int i = start+1 ; i < end ; i++){
//      if((b.getSample( c,i)>0)!=sgn){return i;}
//    }
//
//  }
//  return -1;
//}
//
//void PlayableBuffer::findFadeLoopPoints(){
////  const int zeroSearch = 64;
////  int c = 0;
//
////  int lZ = findFirstZeroCrossing(audioBuffer, recordNeedle, recordNeedle-zeroSearch, c);
////  if(lZ>0){
////    fadeLoopOutPoint = lZ;
////    reverseFadeOut = true;
////  }
////  else{
////    reverseFadeOut = false;
////    fadeLoopOutPoint = recordNeedle - getNumSampleFadeOut();
////  }
//  fadeLoopOutPoint = recordNeedle;
//  reverseFadeOut = true;
//}


bool PlayableBuffer::isFirstPlayingFrameAfterRecord()const{return lastState == BUFFER_RECORDING && state == BUFFER_PLAYING;}
bool PlayableBuffer::isFirstStopAfterRec()const{return lastState == BUFFER_RECORDING && state == BUFFER_STOPPED;}
bool PlayableBuffer::isFirstPlayingFrame()const{return lastState!=BUFFER_PLAYING && state == BUFFER_PLAYING;}
bool PlayableBuffer::isFirstRecordingFrame()const{return lastState!=BUFFER_RECORDING && state == BUFFER_RECORDING;}
bool PlayableBuffer::wasLastRecordingFrame()const{return lastState==BUFFER_RECORDING && state != BUFFER_RECORDING;}
bool PlayableBuffer::isStopping() const{return (lastState != BUFFER_STOPPED  ) && (state==BUFFER_STOPPED);}
bool PlayableBuffer::isStopped() const{return (state==BUFFER_STOPPED);}
bool PlayableBuffer::isRecording() const{return state == BUFFER_RECORDING;}
bool PlayableBuffer::isPlaying() const{return state == BUFFER_PLAYING;}
bool PlayableBuffer::isFirstRecordedFrame() const{return state == BUFFER_RECORDING && (lastState!=BUFFER_RECORDING);}
bool PlayableBuffer::isOrWasPlaying() const{return (state==BUFFER_PLAYING || lastState==BUFFER_PLAYING) &&  recordNeedle>0 &&getAllocatedNumSample()>0;}
bool PlayableBuffer::isOrWasRecording() const{return (state==BUFFER_RECORDING || lastState==BUFFER_RECORDING) && getAllocatedNumSample()>0;}
//bool PlayableBuffer::isRecordingTail() const{return  recordNeedle>0 && !isRecording() && tailRecordNeedle<2*getNumSampleFadeOut();}
//void PlayableBuffer::stopRecordingTail() {tailRecordNeedle = 2*getNumSampleFadeOut();}

void PlayableBuffer::startRecord(){recordNeedle = 0;tailRecordNeedle = 0;multiNeedle.setLoopSize(0);playNeedle=0;globalPlayNeedle=0;originAudioBuffer.setSize(0, 0,false,false,true);}
inline void PlayableBuffer::startPlay(){multiNeedle.setLoopSize(recordNeedle);setPlayNeedle(0);}




void PlayableBuffer::endProcessBlock(){
  lastState = state;
  stateChanged =false;
  sampleOffsetBeforeNewState = 0;
}


void PlayableBuffer::setState(BufferState newState,int _sampleOffsetBeforeNewState){
  //        lastState = state;
  stateChanged |=newState!=state;
  switch (newState){
    case BUFFER_RECORDING:
      startRecord();
      break;
    case BUFFER_PLAYING:
      multiNeedle.setLoopSize(getRecordedLength());
      setPlayNeedle( -_sampleOffsetBeforeNewState );
      globalPlayNeedle = 0;
      break;
    case BUFFER_STOPPED:
      numTimePlayed = 0;
      multiNeedle.fadeAllOut();
      //        setPlayNeedle(0);
      break;
  }
  state = newState;

  // don't use sample offset when stopping as it's already triggering stop fade out
  // messes up the running time
  if(newState!=BUFFER_STOPPED)sampleOffsetBeforeNewState = _sampleOffsetBeforeNewState;
}

PlayableBuffer::BufferState PlayableBuffer::getState() const{return state;}
PlayableBuffer::BufferState PlayableBuffer::getLastState() const{return lastState;}


uint64 PlayableBuffer::getRecordedLength() const{return recordNeedle;}

uint64 PlayableBuffer::getPlayPos() const{return playNeedle;}
uint64 PlayableBuffer::getGlobalPlayPos() const{return globalPlayNeedle;}



int PlayableBuffer::getSampleOffsetBeforeNewState(){return sampleOffsetBeforeNewState;};
int PlayableBuffer::getNumSampleFadeOut() const{return multiNeedle.fadeOutNumSamples;};

void PlayableBuffer::setSampleRate(float sR){sampleRate = sR;};

void PlayableBuffer::fadeInOut(){
  int fadeIn =multiNeedle.fadeInNumSamples;
  int fadeOut = multiNeedle.fadeOutNumSamples;
  auto startBlock = &bufferBlockList.getReference(0);
  startBlock->applyGainRamp( 0, fadeIn, 0.0f, 1.f);

  int lIdx =floor(bufferBlockList.getNumSamples()/bufferBlockList.bufferBlockSize);
  auto endBlock =&bufferBlockList.getReference(lIdx);
  int endPoint = bufferBlockList.getNumSamples() - (lIdx*bufferBlockList.bufferBlockSize);
  jassert(endPoint>0);
  if(endPoint<fadeOut){
    float ratio = endPoint*1.0/fadeOut;
    int firstPart =(fadeOut-endPoint);
    jassertfalse;
    if(lIdx>0){
      auto eendBlock = &bufferBlockList.getReference(lIdx-1);
      eendBlock->applyGainRamp(bufferBlockList.bufferBlockSize-firstPart, firstPart, 1.0f, ratio);
      endBlock->applyGainRamp(0 , endPoint, ratio, 0.0f);
    }
    else{
      endBlock->applyGainRamp(0,endPoint+1,1.0f,0.0f);
      DBG("truncate fadeOut for small buffer");

    }

  }
  else{
    endBlock->applyGainRamp(endPoint-fadeOut, fadeOut+1, 1.0f, 0.0f);
  }


}
#if BUFFER_CAN_STRETCH

void PlayableBuffer::setTimeRatio(const double ratio){
  //  jassert(isOrWasPlaying());
  if(originAudioBuffer.getNumSamples()==0){return;}

#if RT_STRETCH
  //  initRTStretch();
  if(isPlaying()){
    if(!isStretchPending ){
      stretchNeedle = playNeedle * originAudioBuffer.getNumSamples()/getRecordedLength();
      multiNeedle.fadeAllOut();
      fadePendingStretch.startFadeIn();

    }


    pendingTimeStretchRatio = ratio;
    isStretchPending =   ( ratio!=1.0);
  }
#endif

  if(ratio!=1.0){

    ThreadPool * tp = getEngineThreadPool();
    if(tp->contains(stretchJob)){
      ScopedLock lk(stretchJob->jobLock);
      stretchJob->signalJobShouldExit();
      //      int64 mil = Time::currentTimeMillis();
      tp->waitForJobToFinish(stretchJob,-1);
      //      DBG("Waited : " <<(Time::currentTimeMillis() - mil));
    }
    else stretchJob = nullptr;
    stretchJob =new StretcherJob(this,ratio);
    tp->addJob(stretchJob, true);

  }
  else{

    //    for(int i = 0 ; i < originAudioBuffer.getNumChannels() ; i++){
    //      audioBuffer.copyFrom(i, 0, originAudioBuffer, i, 0, originAudioBuffer.getNumSamples());
    //    }
    bufferBlockList.copyFrom(originAudioBuffer, 0);
    setRecordedLength(originAudioBuffer.getNumSamples());


  }



}
#endif

#if RT_STRETCH
void PlayableBuffer::initRTStretch(){
  pendingTimeStretchRatio = 1.0;
  if(sampleRate<=0){
    jassertfalse;
  }
  jassert(blockSize>0);
  RTStretcher = new RubberBandStretcher  (sampleRate,//size_t sampleRate,
                                          bufferBlockList.getAllocatedNumChannels(),//size_t channels,
                                          RubberBandStretcher::OptionProcessRealTime
                                          //                                          | RubberBandStretcher::OptionTransientsMixed
                                          | RubberBandStretcher::OptionTransientsSmooth
                                          //| RubberBandStretcher::OptionPhaseAdaptive
                                          | RubberBandStretcher::OptionThreadingNever
                                          | RubberBandStretcher::OptionWindowStandard

                                          //| RubberBandStretcher::OptionStretchElastic
                                          | RubberBandStretcher::OptionStretchPrecise

                                          //Options options = DefaultOptions,
                                          //double initialTimeRatio = 1.0,
                                          //double initialPitchScale = 1.0
                                          );

  //  RTStretcher->setMaxProcessSize(blockSize);
  RTStretcher->setPitchScale(1.0);
  stretchNeedle = 0;
}

bool PlayableBuffer::processPendingRTStretch(AudioBuffer<float> & b,uint64 time){

  if(isStretchPending ){
    int outNumSample = b.getNumSamples();
    fadePendingStretch.incrementFade(outNumSample);
    double lastFade = fadePendingStretch.getLastFade();
    double curFade = fadePendingStretch.getCurrentFade();
//    jassert(curFade>0 || lastFade>0);
    int originNumSamples = originAudioBuffer.getNumSamples();


    jassert(pendingTimeStretchRatio!=0);




    int numChannels=getNumChannels();


    int available =RTStretcher->available();

    //    if(stretchNeedle!=originNumSamples)   {
    while(available<outNumSample  ){

      int targetLen = (originNumSamples*pendingTimeStretchRatio);
      double curBeat = (time%(targetLen))*1.0/targetLen + 0.0000001;
      double localBeat = stretchNeedle*1.0/originNumSamples;
      if(!fadePendingStretch.isFadingOut()){
        double diff =(localBeat*1.0/curBeat);
        diff=pow(diff,3);
        double adaptStretch=jmin(jmax(0.5,diff),2.0);
        adaptStretch = 1;
        //                DBG(curBeat << " : " << localBeat << " : " <<diff << " : " << adaptStretch);
        RTStretcher->setTimeRatio(pendingTimeStretchRatio*adaptStretch);
      }

      int toProcess =  RTStretcher->getSamplesRequired();
      //      jassert(toProcess>0);
      const float*  inBuf[numChannels];
      AudioBuffer<float> tmpCache(numChannels,toProcess);

      if(stretchNeedle+toProcess>=originNumSamples){
        toProcess =originNumSamples - stretchNeedle;
      }
      if(toProcess>0){
        for(int i = 0 ; i  < numChannels ; i++){
          inBuf[i] = originAudioBuffer.getReadPointer(i)+stretchNeedle;
        }
        RTStretcher->process(inBuf, toProcess, false);
        stretchNeedle+=toProcess;
        if(stretchNeedle>=originNumSamples){
          stretchNeedle-=originNumSamples;
          numTimePlayed++;
        }
      }
      else{
        available =RTStretcher->available();
        jassert(available>outNumSample);
        break;
      }
      available = RTStretcher->available();
      jassert(available>0);


    }



    //    jassert(available>=outNumSample);
    float*  outBuf[numChannels];
    for(int i = 0 ; i  < numChannels; i++){
      outBuf[i] = b.getWritePointer(i);
    }
    RTStretcher->retrieve(outBuf, jmin(outNumSample,available));
    double ratio = jmin(stretchNeedle *1.0/originNumSamples,1.0);

    b.applyGainRamp(0, outNumSample,
                    lastFade,
                    curFade);



    if(!fadePendingStretch.isFadingOut())
      playNeedle = ratio*multiNeedle.loopSize;


    return true;
  }
  return false;

}
void PlayableBuffer::applyStretch(){
  DBG("apply stretch");
  int targetNumSamples = tmpBufferStretch.getNumSamples();
  //  playNeedle =  0;
  setRecordedLength(targetNumSamples);
  bufferBlockList.copyFrom(tmpBufferStretch, 0);
  isStretchReady = false;
  isStretchPending = false;
  pendingTimeStretchRatio = 1.0;
  initRTStretch();
  //  RTStretcher->reset();
  
  
};
#endif

