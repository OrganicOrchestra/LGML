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

PlayableBuffer::PlayableBuffer(int numChannels,int numSamples,int sampleRate,int blockSize):
audioBuffer(numChannels,numSamples),
recordNeedle(0),
startJumpNeedle(0),
playNeedle(0),globalPlayNeedle(0),isJumping(false),
state(BUFFER_STOPPED),
lastState(BUFFER_STOPPED),
stateChanged(false),
numTimePlayed(0),
sampleOffsetBeforeNewState(0),
hasBeenFaded (false),fadeSamples(256),
fadeRecorded(256),
tailRecordNeedle(0),

sampleRate(44100)
#if BUFFER_CAN_STRETCH
,stretchJob(nullptr)
#if RT_STRETCH
,pendingTimeStretchRatio(1)
#endif
#endif
{
  fadeRecorded.setFadedOut();
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
  if (isFirstRecordingFrame()){
    succeeded = writeAudioBlock(buffer, sampleOffsetBeforeNewState);

  }
  else if(isRecording() ){
    succeeded = writeAudioBlock(buffer);
  }
  else if( wasLastRecordingFrame()){
    succeeded = writeAudioBlock(buffer, 0,sampleOffsetBeforeNewState);
//    fadeInOut(fadeSamples, 0);
    originaudioBuffer.setSize(audioBuffer.getNumChannels(), getRecordedLength());
    for(int i = 0 ; i < audioBuffer.getNumChannels() ; i++){
      originaudioBuffer.copyFrom(i, 0, audioBuffer, i, 0, getRecordedLength());
    }

  }

  if( isRecordingTail() ){
    succeeded = writeAudioBlock(buffer);
  }

  buffer.clear();


  if ( isOrWasPlaying()){


    readNextBlock(buffer,time,sampleOffsetBeforeNewState);

#if RT_STRETCH
    processPendingRTStretch(buffer);
#endif

  }
  else if(isStopped()&& playNeedle>0){
    playNeedle = 0;
    startJumpNeedle = 0;
    globalPlayNeedle = 0;
    isJumping = false;
  }

  return succeeded;
}


inline bool PlayableBuffer::writeAudioBlock(const AudioBuffer<float> & buffer, int fromSample,int samplesToWrite){

  samplesToWrite= samplesToWrite==-1?buffer.getNumSamples()-fromSample:samplesToWrite;
  if (recordNeedle + buffer.getNumSamples()> audioBuffer.getNumSamples()) {
    //      jassertfalse;
    return false;
  }
  else{
    const int maxChannel = jmin(audioBuffer.getNumChannels(),buffer.getNumChannels());
    for (int i =  maxChannel- 1; i >= 0; --i) {
      audioBuffer.copyFrom(i, (int)recordNeedle+tailRecordNeedle, buffer, i, fromSample, samplesToWrite );
    }
    if(isRecording()){jassert(tailRecordNeedle ==0);recordNeedle += samplesToWrite;}
    else{tailRecordNeedle+=samplesToWrite;}
  }

  return true;
}


inline void PlayableBuffer::readNextBlock(AudioBuffer<float> & buffer,uint64 time,int fromSample   ){
  if(recordNeedle==0){
    buffer.clear();
    jassertfalse;
    return;
  }

  if(fromSample>0){
    int dbg;
    dbg++;
  }
  int numSamples = buffer.getNumSamples()-fromSample;



  if(state==BUFFER_PLAYING){
    int targetTime = (time + fromSample)%getRecordedLength();
    if(targetTime != playNeedle){
      //      jassertfalse;
      setPlayNeedle(targetTime);

    }
  }


  // assert false for now to check alignement
  if(isFirstPlayingFrame()){

    jassert(playNeedle==0);
  }


  buffer.clear();
  multiNeedle.addToBuffer(audioBuffer, buffer, buffer.getNumSamples(), getRecordedLength());


  playNeedle += numSamples;
  globalPlayNeedle+=numSamples;
  if(playNeedle>=getRecordedLength()){
    numTimePlayed ++;
    
  }
  playNeedle %= getRecordedLength();



}


void PlayableBuffer::setPlayNeedle(int n){
  if(playNeedle!=n){
    if(!isJumping){
      startJumpNeedle = playNeedle;
      //      jumpingNeedle =startJumpNeedle;
    }
    isJumping = true;
  }
  multiNeedle.jumpTo(n);
  playNeedle = n;

}

void PlayableBuffer::cropEndOfRecording(int sampletoRemove){
  jassert(sampletoRemove<recordNeedle);
  recordNeedle-=sampletoRemove;
}
void PlayableBuffer::padEndOfRecording(int sampleToAdd){
  audioBuffer.clear((int)recordNeedle, sampleToAdd);
  recordNeedle+=sampleToAdd;
}
void PlayableBuffer::setSizePaddingIfNeeded(uint64 targetSamples){
  jassert(targetSamples<audioBuffer.getNumSamples());
  if(targetSamples>recordNeedle){
    padEndOfRecording((int)(targetSamples - recordNeedle));
  }
  else if (targetSamples<recordNeedle){
    cropEndOfRecording((int)(recordNeedle - targetSamples));
  }

}

int findFirstZeroCrossing(const AudioBuffer<float> & b, int start,int end,int c){
  float fS = b.getSample(c, start);
  if(fS ==0) return start;
  bool sgn = fS>0;
  if( start>end){
  for(int i = start-1 ; i > end ; i--){
    if((b.getSample(c,i)>0)!=sgn){ return i;}
    }
  }
  else{
    for(int i = start+1 ; i < end ; i++){
      if((b.getSample( c,i)>0)!=sgn){return i;}
    }

  }
  return -1;
}
void PlayableBuffer::fadeInOut(int fadeNumSamples,double mingain){
  const int zeroSearch = 64;
  int c = 0;
  int fZ = findFirstZeroCrossing(audioBuffer, 0, zeroSearch, c);
  if(fZ>0){
    audioBuffer.clear(c, 0, fZ);
  }
  int lZ = findFirstZeroCrossing(audioBuffer, recordNeedle, recordNeedle-zeroSearch, c);
  if(lZ>0){
    audioBuffer.clear(c, lZ+1, recordNeedle- lZ + 1 );
  }
  if (fadeNumSamples>0 ){
    if(getRecordedLength()<2 * fadeNumSamples -1) {fadeNumSamples = (int)getRecordedLength()/2 - 1;}
    for (int i = audioBuffer.getNumChannels() - 1; i >= 0; --i) {
      audioBuffer.applyGainRamp(i, 0, fadeNumSamples, (float)mingain, 1);
      audioBuffer.applyGainRamp(i, (int)recordNeedle - fadeNumSamples, fadeNumSamples, 1, (float)mingain);
    }
  }
}


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
bool PlayableBuffer::isRecordingTail() const{return recordNeedle>0 && !isRecording() && tailRecordNeedle<fadeSamples*2;}

void PlayableBuffer::startRecord(){recordNeedle = 0;tailRecordNeedle = 0;playNeedle=0;}
inline void PlayableBuffer::startPlay(){setPlayNeedle(0);}

bool PlayableBuffer::checkTimeAlignment(uint64 curTime,const int minQuantifiedFraction){

  if(state == BUFFER_PLAYING  && recordNeedle>0){


    int globalPos =(curTime%minQuantifiedFraction);
    int localPos =(playNeedle%minQuantifiedFraction);
    if(globalPos!=localPos){
      jassertfalse;
      if(!isJumping)startJumpNeedle = playNeedle;
      playNeedle = (playNeedle - localPos) + globalPos;
      isJumping = true;



    }
  }

  return !isJumping;
}



void PlayableBuffer::endProcessBlock(){
  lastState = state;
  stateChanged =false;
  isJumping = false;
  //  startJumpNeedle=playNeedle;
  sampleOffsetBeforeNewState = 0;
}


void PlayableBuffer::setState(BufferState newState,int _sampleOffsetBeforeNewState){
  //        lastState = state;
  stateChanged |=newState!=state;
  switch (newState){
    case BUFFER_RECORDING:
      hasBeenFaded = false;
      recordNeedle = 0;
      tailRecordNeedle= 0;
      numTimePlayed = 0;
      startJumpNeedle = 0;
      setPlayNeedle(0);
      globalPlayNeedle = 0;
      break;
    case BUFFER_PLAYING:
      fadeRecorded.startFadeIn();
      setPlayNeedle( 0);
      globalPlayNeedle = 0;
      break;
    case BUFFER_STOPPED:
      numTimePlayed = 0;
      fadeRecorded.startFadeOut();

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


uint64 PlayableBuffer::getStartJumpPos() const{return startJumpNeedle;}
int PlayableBuffer::getSampleOffsetBeforeNewState(){return sampleOffsetBeforeNewState;};
int PlayableBuffer::getNumSampleFadeOut(){return fadeRecorded.fadeOutNumSamples;};

void PlayableBuffer::setSampleRate(float sR){sampleRate = sR;};



void PlayableBuffer::setTimeRatio(const double ratio){
  jassert(isOrWasPlaying());

#if RT_STRETCH
  pendingTimeStretchRatio = ratio;
#endif

  if(ratio!=1.0){

    ThreadPool * tp = getEngineThreadPool();
    if(tp->contains(stretchJob)){
      stretchJob->signalJobShouldExit();
      //      tp->waitForJobToFinish(StretcherJob,-1);
    }
    else stretchJob = nullptr;
    stretchJob =new StretcherJob(this,ratio);
    tp->addJob(stretchJob, true);

  }
  else{
    for(int i = 0 ; i < originaudioBuffer.getNumChannels() ; i++){
      audioBuffer.copyFrom(i, 0, originaudioBuffer, i, 0, originaudioBuffer.getNumSamples());
    }
    recordNeedle = originaudioBuffer.getNumSamples()-1;
//    fadeInOut(fadeSamples, 0);
  }



}


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

