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
#include "RubberBandStretcher.h"
using namespace RubberBand;
#endif

PlayableBuffer::PlayableBuffer(int numChannels,int numSamples,int sampleRate,int blockSize):
loopSample(numChannels,numSamples),
recordNeedle(0),
startJumpNeedle(0),
playNeedle(0),globalPlayNeedle(0),isJumping(false),
state(BUFFER_STOPPED),
lastState(BUFFER_STOPPED),
stateChanged(false),
numTimePlayed(0),
sampleOffsetBeforeNewState(0),
hasBeenFaded (false),fadeSamples(32),
fadeRecorded(32),

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
  loopSample.setSize(n, loopSample.getNumSamples());

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
    fadeInOut(fadeSamples, 0);
    originLoopSample.setSize(loopSample.getNumChannels(), getRecordedLength());
    for(int i = 0 ; i < loopSample.getNumChannels() ; i++){
      originLoopSample.copyFrom(i, 0, loopSample, i, 0, getRecordedLength());
    }





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

    readNextBlock(buffer,time,sampleOffsetBeforeNewState);
    //    }

    double startGain = fadeRecorded.getLastFade();
    double endGain = fadeRecorded.getCurrentFade();
    const int maxChannel = jmin(loopSample.getNumChannels(),buffer.getNumChannels());
    for(int c = 0 ; c <maxChannel ; c++ ){
      buffer.applyGainRamp(c, 0, buffer.getNumSamples(), (float)startGain, (float)endGain);
    }
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

      if(firstSegmentLength>=0 && secondSegmentLength>0){

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
  int fZ = findFirstZeroCrossing(loopSample, 0, zeroSearch, c);
  if(fZ>0){
    loopSample.clear(c, 0, fZ);
  }
  int lZ = findFirstZeroCrossing(loopSample, recordNeedle, recordNeedle-zeroSearch, c);
  if(lZ>0){
    loopSample.clear(c, lZ+1, recordNeedle- lZ + 1 );
  }
  if (fadeNumSamples>0 ){
    if(getRecordedLength()<2 * fadeNumSamples -1) {fadeNumSamples = (int)getRecordedLength()/2 - 1;}
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



////////////////
// stretcher

#if BUFFER_CAN_STRETCH

class StretchJob : public ThreadPoolJob{
public:

  StretchJob(PlayableBuffer * pb,double _ratio):
  ThreadPoolJob("stretch"),
  owner(pb),
  ratio(_ratio)
  {
  };


  JobStatus runJob()override;

  int studyStretch(double ratio,int start,int blockSize);
  void processStretch(int start,int block,int *read,int * produced);
  // stretching function
  void initStretcher(int sR,int c);
  PlayableBuffer * owner;
  double ratio;

  ScopedPointer<RubberBand::RubberBandStretcher> stretcher;

};


void PlayableBuffer::setTimeRatio(const double ratio){
  jassert(isOrWasPlaying());

#if RT_STRETCH
  pendingTimeStretchRatio = ratio;
#endif
  
  if(ratio!=1.0){

    ThreadPool * tp = getEngineThreadPool();
    if(tp->contains(stretchJob)){
      stretchJob->signalJobShouldExit();
      //      tp->waitForJobToFinish(stretchJob,-1);
    }
    else stretchJob = nullptr;
    stretchJob =new StretchJob(this,ratio);
    tp->addJob(stretchJob, true);

  }
  else{
    for(int i = 0 ; i < originLoopSample.getNumChannels() ; i++){
      loopSample.copyFrom(i, 0, originLoopSample, i, 0, originLoopSample.getNumSamples());
    }
    recordNeedle = originLoopSample.getNumSamples()-1;
    fadeInOut(fadeSamples, 0);
  }



}


#if RT_STRETCH
void PlayableBuffer::initRTStretch(int blockSize){
  pendingTimeStretchRatio = 1.0;
  RTStretcher = new RubberBandStretcher  (sampleRate,//size_t sampleRate,
                                          loopSample.getNumChannels(),//size_t channels,
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

ThreadPoolJob::JobStatus StretchJob::runJob(){
  int processed = 0;
  int block = 4096;


  while(!shouldExit() && processed<owner->originLoopSample.getNumSamples()){
    processed+=studyStretch(ratio,processed,block);
  }

  processed = 0;
  int read = 0;
  int produced = 0;
  while(!shouldExit()&& processed<owner->originLoopSample.getNumSamples()){
    processStretch(processed,block,&read,&produced);
    processed+=read;
    if(read==0){
      int dbg;
      dbg++;
      break;
    }

  }
  if(!shouldExit()){

    int targetNumSamples = owner->originLoopSample.getNumSamples()*ratio;

    int diffSample =abs(produced-targetNumSamples);
    if(diffSample>128){
      jassertfalse;
    }

    owner->recordNeedle = produced;
    owner->playNeedle = owner->playNeedle*1.0/owner->recordNeedle*targetNumSamples;
    owner->setSizePaddingIfNeeded(targetNumSamples);
    //    int dbg =stretcher->getSamplesRequired();
    //    jassert(dbg<=0);

    owner->fadeInOut(owner->fadeSamples, 0);

    int dbg=stretcher->available();
    jassert(dbg<=0);
  }
  return jobHasFinished;
}


void StretchJob::initStretcher(int sampleRate,int numChannels){
  if(sampleRate==0){
    sampleRate=44100;
  }
  stretcher=new    RubberBandStretcher(sampleRate,//size_t sampleRate,
                                       numChannels,//size_t channels,
                                       RubberBandStretcher::OptionProcessOffline
                                       | RubberBandStretcher::OptionTransientsMixed
                                       //~ | RubberBandStretcher::OptionTransientsSmooth
                                       //| RubberBandStretcher::OptionPhaseAdaptive
                                       | RubberBandStretcher::OptionThreadingNever
                                       | RubberBandStretcher::OptionWindowStandard

                                       //                                       | RubberBandStretcher::OptionStretchElastic
                                       | RubberBandStretcher::OptionStretchPrecise

                                       //Options options = DefaultOptions,
                                       //double initialTimeRatio = 1.0,
                                       //double initialPitchScale = 1.0
                                       );

  stretcher->setDebugLevel(0);
  stretcher->setPitchScale(1.0);


}

int StretchJob::studyStretch(double ratio,int start,int block){

  if(start==0){
    if(block==-1)block=owner->originLoopSample.getNumSamples();

    initStretcher(owner->sampleRate , owner->loopSample.getNumChannels());
    jassert(isfinite(ratio));
    stretcher->setTimeRatio(ratio);
    stretcher->setExpectedInputDuration(owner->originLoopSample.getNumSamples());
    stretcher->setMaxProcessSize(block);
    //  jassert(stretcher->getInputIncrement() == originLoopSample.getNumSamples());
  }


  bool isFinal =  start+block>=owner->originLoopSample.getNumSamples();
  if(isFinal){
    block -= jmax(0,(start+block)-owner->originLoopSample.getNumSamples());

  }
  const float* tmp[owner->originLoopSample.getNumChannels()];
  for(int i = 0 ; i  < owner->originLoopSample.getNumChannels() ; i++){
    tmp[i] = owner->originLoopSample.getReadPointer(i) + start;
  }
  stretcher->study(tmp, block, isFinal);
  return block;

}
void StretchJob::processStretch(int start,int block,int * read, int * produced){


  if(block==-1)block=owner->originLoopSample.getNumSamples();

  //  int latency = stretcher->getLatency();
  //  jassert (latency == 0);


  bool isFinal = start+block>=owner->originLoopSample.getNumSamples();
  if(isFinal){
    block -= jmax(0,(start+block)-owner->originLoopSample.getNumSamples());

  }

  const float * tmpIn[owner->originLoopSample.getNumChannels()];
  for(int i = 0 ; i  < owner->originLoopSample.getNumChannels() ; i++){
    tmpIn[i] = owner->originLoopSample.getReadPointer(i) + start;
  }




  stretcher->process(tmpIn, block, isFinal);
  int available = stretcher->available();
  jassert( *produced + available< owner->loopSample.getNumSamples());

  float * tmpOut[owner->loopSample.getNumChannels()];
  for(int i = 0 ; i  < owner->loopSample.getNumChannels() ; i++){
    tmpOut[i] = owner->loopSample.getWritePointer(i) + *produced;
  }

  int retrievedSamples = stretcher->retrieve(tmpOut, available);
  jassert(retrievedSamples==available);

  *read = block;
  *produced+=retrievedSamples;

  if(isFinal){
    int dbg=stretcher->available();
    jassert(dbg<=0);
  }

}

#endif /* BUFFER_CAN_STRETCH */

