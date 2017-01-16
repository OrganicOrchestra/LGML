/*
  ==============================================================================

    PlayableBuffer.cpp
    Created: 10 Jan 2017 4:40:14pm
    Author:  Martin Hermant

  ==============================================================================
*/

#include "PlayableBuffer.h"
#include "RubberBandStretcher.h"
#include "AudioDebugPipe.h"
using namespace RubberBand;

PlayableBuffer::PlayableBuffer(int numChannels,int numSamples,int sampleRate,int blockSize):
loopSample(numChannels,numSamples),
recordNeedle(0),
startJumpNeedle(0),
playNeedle(0),isJumping(false),
state(BUFFER_STOPPED),
lastState(BUFFER_STOPPED),
stateChanged(false),
numTimePlayed(0),
sampleOffsetBeforeNewState(0),
hasBeenFaded (false),fadeSamples(380),
fadeRecorded(1000)
{
  fadeRecorded.setFadedOut();
  jassert(numSamples < std::numeric_limits<int>::max());
  initStretcher(sampleRate,numChannels,blockSize);

//  stretcher->setPitchOption(RubberBandStretcher::Option::OptionPitchHighConsistency);
}

PlayableBuffer::~PlayableBuffer(){}


void PlayableBuffer::setNumChannels(int n){
  loopSample.setSize(n, loopSample.getNumSamples());
  
}


void PlayableBuffer::initStretcher(int sampleRate,int numChannels,int blockSize){
  if(sampleRate==0){
    sampleRate=44100;
  }
    stretcher=new    RubberBandStretcher(sampleRate,//size_t sampleRate,
                                         numChannels,//size_t channels,
                                         RubberBandStretcher::OptionProcessOffline
                                         | RubberBandStretcher::OptionTransientsCrisp
                                         //~ | RubberBandStretcher::OptionTransientsSmooth
                                         //| RubberBandStretcher::OptionPhaseAdaptive
                                         | RubberBandStretcher::OptionThreadingNever
                                         | RubberBandStretcher::OptionWindowStandard

                                         //| RubberBandStretcher::OptionStretchElastic
                                                                                | RubberBandStretcher::OptionStretchPrecise

                                         //Options options = DefaultOptions,
                                         //double initialTimeRatio = 1.0,
                                         //double initialPitchScale = 1.0
                                         );
    // max block size
//    stretcher->setMaxProcessSize(blockSize);

}
void PlayableBuffer::setTimeRatio(const double ratio){
//  stretcher->setTimeRatio(1);
//  stretcher->reset();
  initStretcher(44100, 2, 0);
  jassert(isfinite(ratio));
  stretcher->setTimeRatio(ratio);
  stretcher->setExpectedInputDuration(originLoopSample.getNumSamples());
  stretcher->setMaxProcessSize(originLoopSample.getNumSamples());
//  jassert(stretcher->getInputIncrement() == originLoopSample.getNumSamples());

  {
    ScopedLock lk (stretcherLock);
  stretcher->study(originLoopSample.getArrayOfWritePointers(), originLoopSample.getNumSamples(), true);
  }

  if(ratio!=1.0){
    ScopedLock lk (stretcherLock);

    int latency = stretcher->getLatency();
    jassert (latency == 0);

    int required= originLoopSample.getNumSamples();

      stretcher->process(originLoopSample.getArrayOfWritePointers(), required, true);
      int available = stretcher->available();
      jassert( available< loopSample.getNumSamples());

      int retrievedSamples = stretcher->retrieve(originLoopSample.getArrayOfWritePointers(), available);
      int produced=retrievedSamples;


    int targetNumSamples = originLoopSample.getNumSamples()*ratio;

    jassert(abs(produced-targetNumSamples)< 1024);
    playNeedle = playNeedle*1.0/recordNeedle*targetNumSamples;
    setSizePaddingIfNeeded(targetNumSamples);
//    int dbg =stretcher->getSamplesRequired();
//    jassert(dbg<=0);
    int dbg=stretcher->available();
    jassert(dbg<=0);


  }


}
bool PlayableBuffer::processNextBlock(AudioBuffer<float> & buffer){
  bool succeeded = true;
  if (isFirstRecordingFrame()){
    succeeded = writeAudioBlock(buffer, sampleOffsetBeforeNewState);

  }
  else if(isRecording() ){
    succeeded = writeAudioBlock(buffer);
  }
  else if( wasLastRecordingFrame()){
    succeeded = writeAudioBlock(buffer, 0,sampleOffsetBeforeNewState);

    originLoopSample.setSize(loopSample.getNumChannels(), getRecordedLength());
    for(int i = 0 ; i < loopSample.getNumChannels() ; i++){
      originLoopSample.copyFrom(i, 0, loopSample, i, 0, getRecordedLength());
    }
    fadeInOut(fadeSamples, 0);
    stretcher->reset();
    stretcher->setTimeRatio(1.0);
    stretcher->setPitchScale(1.0);
    
//    stretcher->setExpectedInputDuration(getRecordedLength());

  }



  buffer.clear();

  if(isStopping()){
    fadeRecorded.startFadeOut();
  }
  if(isFirstPlayingFrame()){

    fadeRecorded.setFadedOut();
    fadeRecorded.startFadeIn();
  }

  fadeRecorded.incrementFade(buffer.getNumSamples());
  if ( isOrWasPlaying()||fadeRecorded.getLastFade()>0){
    //    if(isOrWasPlaying()){
    readNextBlock(buffer,sampleOffsetBeforeNewState);
    //    }

    double startGain = fadeRecorded.getLastFade();
    double endGain = fadeRecorded.getCurrentFade();
    const int maxChannel = jmin(loopSample.getNumChannels(),buffer.getNumChannels());
    for(int c = 0 ; c <maxChannel ; c++ ){
      buffer.applyGainRamp(c, 0, buffer.getNumSamples(), (float)startGain, (float)endGain);
    }

  }

  return succeeded;
}


inline bool PlayableBuffer::writeAudioBlock(const AudioBuffer<float> & buffer, int fromSample,int samplesToWrite){

  samplesToWrite= samplesToWrite==-1?buffer.getNumSamples()-fromSample:samplesToWrite;
  if (recordNeedle + buffer.getNumSamples()> loopSample.getNumSamples()) {
    //      jassertfalse;
    return false;
  }
  else{
    const int maxChannel = jmin(loopSample.getNumChannels(),buffer.getNumChannels());
    for (int i =  maxChannel- 1; i >= 0; --i) {
      loopSample.copyFrom(i, (int)recordNeedle, buffer, i, fromSample, samplesToWrite );
    }
    recordNeedle += samplesToWrite;
  }

  return true;
}


inline void PlayableBuffer::readNextBlock(AudioBuffer<float> & buffer,int fromSample   ){
  if(recordNeedle==0){
    buffer.clear();
    jassertfalse;
    return;
  }


  int numSamples = buffer.getNumSamples()-fromSample;





  // assert false for now to check alignement
  if(isFirstPlayingFrame()){
    jassert(playNeedle==0);
  }


  // stitch audio jumps by quick fadeIn/Out
  if(isJumping && playNeedle!=startJumpNeedle && state!=BUFFER_STOPPED){
    //LOG("a:jump "<<startJumpNeedle <<","<< playNeedle);

    const int halfBlock =  numSamples/2;
    for (int i = buffer.getNumChannels() - 1; i >= 0; --i) {
      const int maxChannelFromRecorded = jmin(loopSample.getNumChannels() - 1, i);
      buffer.copyFrom(i, fromSample, loopSample, maxChannelFromRecorded, (int)startJumpNeedle, halfBlock);
      buffer.applyGainRamp(i, fromSample, halfBlock, 1.0f, 0.0f);
      buffer.copyFrom(i, fromSample+halfBlock, loopSample, maxChannelFromRecorded, (int)playNeedle+halfBlock, halfBlock);
      buffer.applyGainRamp(i, fromSample+halfBlock-1, halfBlock, 0.0f, 1.0f);

    }
  }

  else{
    if ((playNeedle + numSamples) > recordNeedle)
    {


      int firstSegmentLength =(int)(recordNeedle - playNeedle);
      int secondSegmentLength = numSamples - firstSegmentLength;

      if(firstSegmentLength>0 && secondSegmentLength>0){

        const int maxChannelFromRecorded = jmin(loopSample.getNumChannels() , buffer.getNumChannels());
        for (int i = maxChannelFromRecorded - 1; i >= 0; --i) {
          buffer.copyFrom(i, fromSample, loopSample, i, (int)playNeedle, firstSegmentLength);
          buffer.copyFrom(i, fromSample+firstSegmentLength, loopSample, i, 0, secondSegmentLength);
        }

      }
      else{jassertfalse;}


    }
    else{
      const int maxChannelFromRecorded = jmin(loopSample.getNumChannels() , buffer.getNumChannels());
      for (int i = maxChannelFromRecorded - 1; i >= 0; --i) {
        buffer.copyFrom(i, fromSample, loopSample, i, (int)playNeedle, numSamples);
      }
    }
  }



  playNeedle += numSamples;
  globalPlayNeedle+=numSamples;
  if(playNeedle>=recordNeedle){
    numTimePlayed ++;
  }
  playNeedle %= recordNeedle;


}


void PlayableBuffer::setPlayNeedle(int n){
  if(playNeedle!=n){
    if(!isJumping){
      startJumpNeedle = playNeedle;
    }
    isJumping = true;
  }

  playNeedle = n;

}

void PlayableBuffer::cropEndOfRecording(int sampletoRemove){
  jassert(sampletoRemove<recordNeedle);
  recordNeedle-=sampletoRemove;
}
void PlayableBuffer::padEndOfRecording(int sampleToAdd){
  loopSample.clear((int)recordNeedle, sampleToAdd);
  recordNeedle+=sampleToAdd;
}
void PlayableBuffer::setSizePaddingIfNeeded(uint64 targetSamples){
  jassert(targetSamples<loopSample.getNumSamples());
  if(targetSamples>recordNeedle){
    padEndOfRecording((int)(targetSamples - recordNeedle));
  }
  else if (targetSamples<recordNeedle){
    cropEndOfRecording((int)(recordNeedle - targetSamples));
  }

}

void PlayableBuffer::fadeInOut(int fadeNumSamples,double mingain){
  if (fadeNumSamples>0 ){
    if(recordNeedle<2 * fadeNumSamples -1) {fadeNumSamples = (int)recordNeedle/2 - 1;}
    for (int i = loopSample.getNumChannels() - 1; i >= 0; --i) {
      loopSample.applyGainRamp(i, 0, fadeNumSamples, (float)mingain, 1);
      loopSample.applyGainRamp(i, (int)recordNeedle - fadeNumSamples, fadeNumSamples, 1, (float)mingain);
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
bool PlayableBuffer::isOrWasPlaying() const{return (state==BUFFER_PLAYING || lastState==BUFFER_PLAYING) &&  recordNeedle>0 && loopSample.getNumSamples();}
bool PlayableBuffer::isOrWasRecording() const{return (state==BUFFER_RECORDING || lastState==BUFFER_RECORDING) && loopSample.getNumSamples();}


void PlayableBuffer::startRecord(){recordNeedle = 0;playNeedle=0;}
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
  startJumpNeedle=playNeedle;
  sampleOffsetBeforeNewState = 0;
}


void PlayableBuffer::setState(BufferState newState,int _sampleOffsetBeforeNewState){
  //        lastState = state;
  stateChanged |=newState!=state;
  switch (newState){
    case BUFFER_RECORDING:
      hasBeenFaded = false;
      recordNeedle = 0;
      numTimePlayed = 0;
      startJumpNeedle = 0;
      setPlayNeedle(0);
      globalPlayNeedle = 0;
      break;
    case BUFFER_PLAYING:
      fadeRecorded.startFadeIn();
      setPlayNeedle( 0);
      break;
    case BUFFER_STOPPED:
      numTimePlayed = 0;
      fadeRecorded.startFadeOut();
      globalPlayNeedle = 0;
      //        setPlayNeedle(0);
      break;
  }
  state = newState;
  sampleOffsetBeforeNewState = _sampleOffsetBeforeNewState;
}

PlayableBuffer::BufferState PlayableBuffer::getState() const{return state;}
PlayableBuffer::BufferState PlayableBuffer::getLastState() const{return lastState;}


uint64 PlayableBuffer::getRecordedLength() const{return recordNeedle;}

uint64 PlayableBuffer::getPlayPos() const{return playNeedle;}
uint64 PlayableBuffer::getGlobalPlayPos() const{return globalPlayNeedle;}


uint64 PlayableBuffer::getStartJumpPos() const{return startJumpNeedle;}
int PlayableBuffer::getSampleOffsetBeforeNewState(){return sampleOffsetBeforeNewState;};
int PlayableBuffer::getNumSampleFadeOut(){return fadeRecorded.fadeOutNumSamples;};
