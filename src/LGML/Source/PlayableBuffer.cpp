
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
#include "RubberBandStretcher.h"
using namespace RubberBand;
#endif

PlayableBuffer::PlayableBuffer(int numChannels,int numSamples,int /*sampleRate*/,int /*blockSize*/):
audioBuffer(numChannels,numSamples),
recordNeedle(0),

playNeedle(0),globalPlayNeedle(0),
state(BUFFER_STOPPED),
lastState(BUFFER_STOPPED),
stateChanged(false),
numTimePlayed(0),
sampleOffsetBeforeNewState(0),
multiNeedle(128,128),
//tailRecordNeedle(0),

sampleRate(44100)
#if BUFFER_CAN_STRETCH
,stretchJob(nullptr)
#if RT_STRETCH
,pendingTimeStretchRatio(1)
#endif
#endif
{

  jassert(numSamples < std::numeric_limits<int>::max());
#if RT_STRETCH
  initRTStretch(blockSize);
#endif
}

PlayableBuffer::~PlayableBuffer(){

}


void PlayableBuffer::setNumChannels(int n){
  audioBuffer.setSize(n, audioBuffer.getNumSamples());

}




bool PlayableBuffer::processNextBlock(AudioBuffer<float> & buffer,uint64 time){
  bool succeeded = true;


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
    //    fadeInOut(fadeSamples, 0);


  }

//  if( isRecordingTail() ){
//    succeeded &= writeAudioBlock(buffer,sampleOffsetBeforeNewState,-1,true);
//  }

  // save to buffer when end recording
  if( wasLastRecordingFrame() && recordNeedle>0 && originAudioBuffer.getNumSamples()==0){
    originAudioBuffer.setSize(audioBuffer.getNumChannels(), getRecordedLength());
    for(int i = 0 ; i < audioBuffer.getNumChannels() ; i++){
      originAudioBuffer.copyFrom(i, 0, audioBuffer, i, 0, getRecordedLength());
    }

  }

  buffer.clear();


  //  if ( isOrWasPlaying()){


  readNextBlock(buffer,time,sampleOffsetBeforeNewState);

#if RT_STRETCH
  processPendingRTStretch(buffer);
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


  if (recordNeedle + buffer.getNumSamples()> audioBuffer.getNumSamples()) {
    return false;
  }
  else{
    const int maxChannel = jmin(audioBuffer.getNumChannels(),buffer.getNumChannels());
    for (int i =  maxChannel- 1; i >= 0; --i) {
      audioBuffer.copyFrom(i, (int)recordNeedle, buffer, i, fromSample, samplesToWrite );
    }

      recordNeedle += samplesToWrite;
  }

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



  if(state==BUFFER_PLAYING){
    int targetTime = (time  + getRecordedLength())%getRecordedLength();
    if(targetTime != playNeedle){
      //      jassertfalse;
      setPlayNeedle(targetTime);

    }
  }


  if(wasLastRecordingFrame()){
    multiNeedle.setLoopSize(getRecordedLength());

  }
  // assert false for now to check alignement
  if(isFirstPlayingFrame()){

#if !LINK_SUPPORT
    jassert(playNeedle==0);
#endif
  }


  buffer.clear();
  if(recordNeedle>0 && !isRecording()){
    if(isPlaying())jassert(multiNeedle.currentPos==playNeedle);

      multiNeedle.addToBuffer(audioBuffer, buffer, buffer.getNumSamples(), isPlaying());
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
void PlayableBuffer::padEndOfRecording(int sampleToAdd){
  audioBuffer.clear((int)recordNeedle, sampleToAdd);
  recordNeedle+=sampleToAdd;
  multiNeedle.setLoopSize(recordNeedle);
}
void PlayableBuffer::setRecordedLength(uint64 targetSamples){
  jassert(targetSamples<=audioBuffer.getNumSamples());
  recordNeedle = targetSamples;
  multiNeedle.setLoopSize(targetSamples);
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
bool PlayableBuffer::isOrWasPlaying() const{return (state==BUFFER_PLAYING || lastState==BUFFER_PLAYING) &&  recordNeedle>0 && audioBuffer.getNumSamples();}
bool PlayableBuffer::isOrWasRecording() const{return (state==BUFFER_RECORDING || lastState==BUFFER_RECORDING) && audioBuffer.getNumSamples();}
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

#if BUFFER_CAN_STRETCH

void PlayableBuffer::setTimeRatio(const double ratio){
//  jassert(isOrWasPlaying());
  if(originAudioBuffer.getNumSamples()==0){return;}

#if RT_STRETCH
  pendingTimeStretchRatio = ratio;
#endif

  if(ratio!=1.0){

    ThreadPool * tp = getEngineThreadPool();
    if(tp->contains(stretchJob)){
      ScopedLock lk(stretchJob->jobLock);
      stretchJob->signalJobShouldExit();
      tp->waitForJobToFinish(stretchJob,-1);
    }
    else stretchJob = nullptr;
    stretchJob =new StretcherJob(this,ratio);
    tp->addJob(stretchJob, true);

  }
  else{

    for(int i = 0 ; i < originAudioBuffer.getNumChannels() ; i++){
      audioBuffer.copyFrom(i, 0, originAudioBuffer, i, 0, originAudioBuffer.getNumSamples());
    }
    setRecordedLength(originAudioBuffer.getNumSamples());

  }



}
#endif

#if RT_STRETCH
void PlayableBuffer::initRTStretch(int blockSize){
  pendingTimeStretchRatio = 1.0;
  RTStretcher = new RubberBandStretcher  (sampleRate,//size_t sampleRate,
                                          audioBuffer.getNumChannels(),//size_t channels,
                                          RubberBandStretcher::OptionProcessRealTime
                                          | RubberBandStretcher::OptionTransientsMixed
                                          // | RubberBandStretcher::OptionTransientsSmooth
                                          //| RubberBandStretcher::OptionPhaseAdaptive
                                          | RubberBandStretcher::OptionThreadingNever
                                          | RubberBandStretcher::OptionWindowStandard

                                          //| RubberBandStretcher::OptionStretchElastic
                                          //                                          | RubberBandStretcher::OptionStretchPrecise

                                          //Options options = DefaultOptions,
                                          //double initialTimeRatio = 1.0,
                                          //double initialPitchScale = 1.0
                                          );

  RTStretcher->setMaxProcessSize(blockSize);
}

void PlayableBuffer::processPendingRTStretch(AudioBuffer<float> & b){
  if(pendingTimeStretchRatio!=1.0){
    
  }
  
}
void PlayableBuffer::applyStretch(){
  
  
  pendingTimeStretchRatio = 1.0;
};
#endif

