/*
 ==============================================================================

 TimeManager.cpp
 Created: 2 Mar 2016 8:33:44pm
 Author:  bkupe

 ==============================================================================
 */
#ifndef _USE_MATH_DEFINES
#define _USE_MATH_DEFINES
#include <cmath>
#endif


#include "TimeManager.h"



juce_ImplementSingleton(TimeManager);

#include "NodeBase.h"
#include "DebugHelpers.h"
#include "AudioHelpers.h"


TimeManager::TimeManager():
beatTimeInSample(1000),
sampleRate(44100),
blockSize(0),
ControllableContainer("Time"),
beatTimeGuessRange(.4,.85),
BPMRange(40,250),
_isLocked(false),
currentBeatPeriod(.5),
lastTaped(0),
tapInRow(0),
hasJumped(false),
hadMasterCandidate(false),
timeMasterCandidate(nullptr),
samplePerBeatGranularity(8),
audioClock(0)
#if LINK_SUPPORT
,linkSession(120.0),
linkTimeLine(ableton::link::Timeline(),true),
linkLatency(00)
#endif

{

  BPM = addFloatParameter("bpm","current BPM",120,(float)BPMRange.getStart(), (float)BPMRange.getEnd());
  BPM->isCommitableParameter = true;
  playState = addBoolParameter("PlayStop", "play or stop global transport", false);
  BPMLocked = addBoolParameter("bpmLocked", "bpm is locked by somebody", false);
  BPMLocked->isControllableFeedbackOnly = true;
  isSettingTempo = addBoolParameter("isSettingTempo", "is someone setting tempo (recording first loop)", false);
  currentBar  = addIntParameter("currentBar", "currentBar in transport", 0, 0, 9999999);
  currentBeat  = addIntParameter("currentBeat", "currentBeat in transport", 0, 0, 999999);
  beatPerBar = addIntParameter("beatPerBar", "beat Per Bar", 4, 1, 8);
  playTrigger = addTrigger("play", "trigger play");
  stopTrigger = addTrigger("stop", "trigger stop");
  quantizedBarFraction = addIntParameter("globalQuantization", "Global quantization in fraction of a bar", 1, 0, 16);
  tapTempo = addTrigger("tapTempo", "tap at least 2 times to set the tempo");
  click = addBoolParameter("Metronome", "Play the metronome click", false);
  clickVolume = addFloatParameter("Metronome Volume", "Click's volume if metronome is active", .5f, 0, 1);
  setBPMInternal(BPM->doubleValue(),false);

  linkEnabled = addBoolParameter("link enabled","activate link",false);
  linkEnabled->enabled = LINK_SUPPORT;

  linkNumPeers = addIntParameter("linkNumPeers","number of connected link devices",0,0,32);
  linkNumPeers->enabled = LINK_SUPPORT;
  linkNumPeers->isControllableFeedbackOnly = true;
  linkNumPeers->isEditable = false;
  

#if LINK_SUPPORT
  linkEnabled->setValue(false);
  linkSession.setNumPeersCallback(&TimeManager::linkNumPeersCallBack);

  linkSession.setTempoCallback(&TimeManager::linkTempoCallBack);
  linkLatencyParam = addFloatParameter("linkLatency", "link latency to add for lgml", 10, -30, 80);
#endif

  clickFader = new FadeInOut(10000,10000,true,1.0/3.0);


}
TimeManager::~TimeManager()
{

}


void TimeManager::incrementClock(int block){
  audioClock+=block;
  jassert(blockSize!=0);
  if(block!=blockSize){
#if !LGML_UNIT_TESTS
    jassertfalse;
#endif
    setBlockSize(block);

  }
#if LINK_SUPPORT
  linkTimeLine = linkSession.captureAudioTimeline();
#endif

  updateState();
  if(_isLocked){

    return;
  }

  checkCommitableParams();
  hasJumped = updateAndNotifyTimeJumpedIfNeeded();


  if(!hasJumped && timeState.isPlaying ){
    timeState.time+=blockSize;
  }
  timeState.nextTime = timeState.time+blockSize;
  int lastBeat =  int(currentBeat->doubleValue());
  int newBeat = getBeatInt();
  if(lastBeat!=newBeat){
    currentBeat->setValue(newBeat);
    if(newBeat%((int)beatPerBar->value) == 0){
      currentBar->setValue(getBar());

    }
  }

#if LINK_SUPPORT
  if(hasJumped && isPlaying() ){
    linkTimeLine.requestBeatAtTime(getBeat(),
                                   //                      std::chrono::system_clock::now().time_since_epoch(),
                                   linkTime,
                                   beatPerBar->intValue()*1.0/quantizedBarFraction->intValue());
    linkSession.commitAudioTimeline(linkTimeLine);

  }

  if(linkEnabled->boolValue() && !timeMasterCandidate){
    linkTime =linkFilter.sampleTimeToHostTime(audioClock) + linkLatency;
    linkTimeLine = linkSession.captureAudioTimeline();
    const int quantum = beatPerBar->intValue()/quantizedBarFraction->doubleValue();
    const double linkBeat = linkTimeLine.beatAtTime(linkTime, quantum);
    //    auto phaseAtTime = linkTimeLine.phaseAtTime(linkTime, tstQ);
    //    auto localPhase = fmod(localBeat,tstQ);
    const double localBeat = getBeat();
    const float driftMs =(linkBeat -localBeat)*beatTimeInSample*1000.0f/sampleRate;

    // we are not using quantified launch atm (beatTime<0), we assume that a link session is always playing and set it only if a timeMasterCandidate sets it
    if((isPlaying() )
       //       && !isFirstPlayingFrame()
       && fabs(driftMs)>1
       ){
      SLOG("link drift : " + String(driftMs)+"ms");
      goToTime(linkBeat  * beatTimeInSample,true);
    }
  }
  
#endif


  desiredTimeState =timeState;
  if(hadMasterCandidate && !isSettingTempo->boolValue()){
    timeMasterCandidate=nullptr;
  }
  hadMasterCandidate = timeMasterCandidate!=nullptr;

  pushCommitableParams();


}
void TimeManager::checkCommitableParams(){
  if(BPM->hasCommitedValue){
    BPM->pushValue(true);
    setBPMInternal(BPM->doubleValue(),true);
    clickFader->startFadeOut();
#if LINK_SUPPORT
    linkTimeLine = linkSession.captureAudioTimeline();
    linkTimeLine.setTempo(BPM->doubleValue(),linkTime);
    linkSession.commitAudioTimeline(linkTimeLine);

#endif
  }
}

void TimeManager::pushCommitableParams(){
  //  BPM->pushValue(true);
}

bool TimeManager::updateAndNotifyTimeJumpedIfNeeded(){

  if(timeState.isJumping){
    jassert(blockSize!=0);
    timeState.time=timeState.nextTime;
    timeState.nextTime = timeState.time+blockSize;
    timeState.isJumping=false;
    timeManagerListeners.call(&TimeManagerListener::timeJumped,timeState.time);
    desiredTimeState =timeState;

    return true;
  }
  return false;
}

void TimeManager::audioDeviceIOCallback (const float** /*inputChannelData*/,
                                         int /*numInputChannels*/,
                                         float** outputChannelData,
                                         int numOutputChannels,
                                         int numSamples) {

  if(click->boolValue()&&timeState.isPlaying && timeState.time>=0){
    static uint64 sinCount = 0;

    bool isFirstBeat = (getClosestBeat()%beatPerBar->intValue()) == 0;
    const int sinFreq = sampleRate /(isFirstBeat?1320:880);
    //    const int sinPeriod = sampleRate / sinFreq;
    //    const double k = 40.0;



    if(desiredTimeState.isJumping){
      clickFader->startFadeOut();
    }
    double cVol = clickVolume->floatValue();

    for(int i = 0 ; i < numSamples;i++){

      double x = (fmod(getBeatInNextSamples(i),1.0) )*beatTimeInSample*1.0/sampleRate ;
      double h = jmax(0.0,jmin(1.0,(0.02 - x)/0.02));

      if((timeState.time+i)%beatTimeInSample==0){
        sinCount = 0;
      }
      double carg = sinCount*1.0/sinFreq;



      clickFader->incrementFade();
      double cFade = clickFader->getCurrentFade();

      double env = cVol*cFade*jmax(0.0,h);//*exp(1.0-h));

      float res = ( env* (sin(2.0*M_PI*carg )+0.1*sin(2.0*M_PI*4.0*carg )));

      for(int c = 0 ;c < numOutputChannels ; c++ ){outputChannelData[c][i] = res;}

      sinCount = (sinCount+1)%(sinFreq);
      //      DBG(clickFader->getCurrentFade());

    }
  }
  else{
    for (int i = 0; i < numOutputChannels; ++i)
      zeromem (outputChannelData[i], sizeof (float) * (size_t) numSamples);
  }


#if !LGML_UNIT_TESTS
  incrementClock(numSamples);
#endif

}

bool TimeManager::isPlaying(){
  return timeState.isPlaying;
}
bool TimeManager::isFirstPlayingFrame(){
  return desiredTimeState.isPlaying && !timeState.isPlaying;
}
bool TimeManager::isJumping(){
  return timeState.isJumping;
}

bool TimeManager::askForBeingMasterCandidate(TimeMasterCandidate * n){

  if(timeMasterCandidate==nullptr && !isAnyoneBoundToTime() ){
    timeMasterCandidate = n;
    isSettingTempo->setValue(true,false,false,true);
    return true;
  }

  return false;
}

bool TimeManager::isMasterCandidate(TimeMasterCandidate * n){
  return  n==timeMasterCandidate;
}
bool TimeManager::hasMasterCandidate(){
  return timeMasterCandidate!=nullptr;
}
void TimeManager::releaseMasterCandidate(TimeMasterCandidate *n){
  jassert(timeMasterCandidate == n);
  isSettingTempo->setValue(false);
}
void TimeManager::releaseIfMasterCandidate(TimeMasterCandidate *n){
  if(n==timeMasterCandidate){
    isSettingTempo->setValue(false);
    n=nullptr;
  }
}

void TimeManager::play(bool _shouldPlay){
  if(!_shouldPlay){
    shouldStop();
    clickFader->startFadeOut();
  }
  else{
    shouldGoToZero();
    shouldPlay();
    clickFader->startFadeIn();
  }
}

void TimeManager::onContainerParameterChanged(Parameter * p){
  if(p==playState){
    play(playState->boolValue());

  }

  else if (p==BPMLocked){
    BPM->isEditable = !BPMLocked->boolValue();
  }



  else if (p==linkEnabled){
#if LINK_SUPPORT
    linkSession.enable(linkEnabled->boolValue());
#endif
  }
  else if (p==linkLatencyParam){
    linkLatency = std::chrono::microseconds((long long)(linkLatencyParam->doubleValue()*1000));
  }

};

void TimeManager::shouldStop(bool now){
  desiredTimeState.isPlaying = false;
  goToTime(0,now);

}

void TimeManager::shouldRestart(bool playing){
  desiredTimeState.isPlaying=playing;
  goToTime( 0);
}
void TimeManager::shouldGoToZero(){
  goToTime(0);
}
void TimeManager::advanceTime(uint64 a,bool now){
  if(now){
    goToTime(desiredTimeState.nextTime+a,true);
  }
  else{
    if(desiredTimeState.isJumping){desiredTimeState.nextTime+=a;}
    else                          {desiredTimeState.jumpTo(timeState.nextTime+a);}
  }

}

void TimeManager::jump(int amount){
  goToTime(timeState.time+amount);


}
void TimeManager::goToTime(long long time,bool now){

  desiredTimeState.jumpTo(time);
  if(now ){

    updateState();
    updateAndNotifyTimeJumpedIfNeeded();
  }

}


void TimeManager::shouldPlay(bool now){
  desiredTimeState.isPlaying = true;
  if(now){
    timeState.isPlaying= true;
    updateState();
  }
}

void TimeManager::updateState(){
  String dbg;
  if(timeState.isPlaying != desiredTimeState.isPlaying){
    dbg+="play:"+String(timeState.isPlaying)+"/"+String(desiredTimeState.isPlaying);
    timeManagerListeners.call(&TimeManagerListener::playStop,desiredTimeState.isPlaying);
  }
  if(timeState.time != desiredTimeState.time){
    dbg+=" ::: time:"+String(timeState.time)+"/"+String(desiredTimeState.time);
    timeManagerListeners.call(&TimeManagerListener::timeJumped,desiredTimeState.time);
  }
  if(desiredTimeState.isJumping){
    int delta = (int)desiredTimeState.nextTime - (int)timeState.nextTime;
    if(abs(delta)>32){
      dbg+="time jumping to : " + String(desiredTimeState.nextTime) + " delta="+String(delta);
    }
    timeManagerListeners.call(&TimeManagerListener::timeJumped,desiredTimeState.time);
  }
  if(dbg!=""){
    LOG(dbg);
  }





  timeState = desiredTimeState;
}

void TimeManager::onContainerTriggerTriggered(Trigger * t) {
  if(t == playTrigger){
    if(!playState->boolValue())playState->setValue(true);
    shouldRestart(true);

    //    desiredTimeState.jumpTo(0);
    //    playState->setValue(false);

  }
  else if(t==stopTrigger){
    playState->setValue(false);
    isSettingTempo->setValue(false,false,false,true);
  }

  else if (t == tapTempo)
  {
    if(!BPMLocked->boolValue())
    {

      if(!playState->boolValue())
      {
        playState->setValue(true);
        currentBeatPeriod = (uint64)0;
      }

      uint64 currentTime = Time().getMillisecondCounter();//timeState.time;
      uint64 delta = currentTime-lastTaped;
      lastTaped = currentTime;
      if((delta>100) && (delta<1000)){
        //        const int maxTapInRow = 4;
        tapInRow = (tapInRow+1)%beatPerBar->intValue();
        const double alpha =.25;// 1. - tapInRow/maxTapInRow *.75 ;
        currentBeatPeriod = alpha*delta + (1-alpha)*currentBeatPeriod;

        //        int targetBeatInt = tapInRow;

        double targetBeat =getBeat();
        if(targetBeat - (int)targetBeat>0.5){targetBeat+=1;}
        int targetBeatInt = floor(targetBeat);

        //        int targetBeatInt = ceil(targetBeat);

        goToTime(targetBeatInt*currentBeatPeriod*sampleRate/1000.0);//(deltaBeat*beatTimeInSample);

        BPM->setValue(60000.0/currentBeatPeriod);
        DBG(targetBeatInt << "," << BPM->doubleValue());



      }
      else if (tapInRow>0){tapInRow --;}

    }

  }
}
void TimeManager::togglePlay(){
  if(playState->boolValue()){
    stopTrigger->trigger();
  }
  else
    playTrigger->trigger();
}



void TimeManager::setSampleRate(int sr){
  sampleRate = sr;
  // actualize beatTime in sample
  beatTimeInSample = (uint64)(sampleRate*1.0 / BPM->doubleValue() *60.0);
}


void TimeManager::setBlockSize(int bS){
  jassert(bS!=0);
  blockSize = bS;



}
void TimeManager::setBPMInternal(double /*_BPM*/,bool adaptTimeInSample){
  isSettingTempo->setValue(false,false,false,true);
  int newBeatTime = (uint64)(sampleRate *1.0/ BPM->doubleValue()*60.0);

  if(adaptTimeInSample){
    uint64 targetTime = timeState.time*newBeatTime/beatTimeInSample;
    goToTime(targetTime);
  }
  beatTimeInSample =newBeatTime;


}
long long TimeManager::getTimeInSample(){
  return timeState.time;
}
long long TimeManager::getNextTimeInSample(){
  if(desiredTimeState.isJumping)return desiredTimeState.nextTime;
  else return  timeState.nextTime;
}
bool  TimeManager::willRestart(){
  return (timeState.nextTime!=0) && (desiredTimeState.nextTime==0);
}





TransportTimeInfo TimeManager::findTransportTimeInfoForLength(uint64 time,double _sampleRate){
  TransportTimeInfo res;
  if(_sampleRate>0){
    res.sampleRate = _sampleRate;
  }
  else{
    res.sampleRate = sampleRate;
  }
  res.barLength = 1;
  double time_seconds = time* 1.0/ res.sampleRate;
  res.beatTime = time_seconds* 1.0/beatPerBar->intValue();

  // over 150 bpm (0.4s)
  while(res.beatTime < beatTimeGuessRange.getStart()){res.beatTime*=2.0;res.barLength/=2.0;}
  // under 70 bpm (0.85s)
  while(res.beatTime > beatTimeGuessRange.getEnd() ){res.beatTime/=2.0;res.barLength*=2.0;}
  res.beatInSample = (res.beatTime*res.sampleRate);

  res.makeValidForGranularity(samplePerBeatGranularity);

  res.bpm = 60.0/res.beatTime;
  DBG("found beat Sample : " << String(res.beatInSample) << " : " << time);

  return res;
}
void TimeManager::setBPMFromTransportTimeInfo(const TransportTimeInfo & info,bool adaptTimeInSample){

  BPM->setValue(info.bpm,false,false,false);
  //  uint64 targetTime = getTimeInSample();

  if(adaptTimeInSample){
    uint64 targetTime = timeState.time*info.beatInSample/beatTimeInSample;
    goToTime(targetTime);
  }
  // force exact beatTimeInSample
  beatTimeInSample = info.beatInSample;
  timeManagerListeners.call(&TimeManagerListener::BPMChanged,BPM->doubleValue());

#if LINK_SUPPORT

  linkTimeLine =ableton::Link::Timeline(ableton::link::Timeline(),true);

  linkTimeLine.setTempo(info.bpm, linkTime);
  linkTimeLine.forceBeatAtTime(0,linkTime,0);

  linkSession.commitAudioTimeline(linkTimeLine);
#endif
  //jassert((int)(barLength*beatPerBar->intValue())>0);

  // lockBPM for now


  //  BPMLocked->setValue(true);


}

uint64 TimeManager::getNextGlobalQuantifiedTime(){
  if(willRestart())return 0;
  return getNextQuantifiedTime(quantizedBarFraction->intValue());
}
uint64 TimeManager::getNextQuantifiedTime(int barFraction){
  if(willRestart())return 0;
  if (barFraction==-1){barFraction=quantizedBarFraction->intValue();}
  if(barFraction==0){return timeState.time;}

  const double samplesPerUnit = (beatTimeInSample*beatPerBar->intValue()*1.0/barFraction);
  const uint64 res = (uint64) ((floor(timeState.time*1.0/samplesPerUnit) + 1)*samplesPerUnit);
  return res;
}

uint64 TimeManager::getTimeForNextBeats(int beats){return (getBeatInt()+ beats)*beatTimeInSample;}

int     TimeManager::getBeatInt()   {return (int)floor(getBeat());}
double  TimeManager::getBeat()      {return (double)(timeState.time*1.0/beatTimeInSample);}
int     TimeManager::getClosestBeat(){return (int)floor(getBeat()+0.5);}
double  TimeManager::getBeatPercent() {return (double)(timeState.time*1.0/beatTimeInSample-getBeatInt());}
double  TimeManager::getBeatInNextSamples(int numSamplesToAdd){return ((double)(timeState.time+numSamplesToAdd)*1.0/beatTimeInSample);}
double TimeManager::getBeatForQuantum(const double q){return floor(getBeat()/q)*q;}

int TimeManager::getBar(){return (int)(floor(getBeatInt()*1.0/beatPerBar->intValue() ));}

void TimeManager::lockTime(bool s){
  _isLocked = s;
}
bool TimeManager::isLocked(){
  return _isLocked;
}
bool TimeManager::getCurrentPosition (CurrentPositionInfo& result){
  result.bpm = BPM->doubleValue();
  result.isPlaying = playState->boolValue();
  result.isRecording = isSettingTempo->boolValue();

  result.ppqPosition = (double)(getBeat()/beatPerBar->intValue());
  result.ppqPositionOfLastBarStart = (double)(getBar()*beatPerBar->intValue());
  result.ppqLoopStart = 0;
  result.ppqLoopEnd = 0;
  result.timeSigNumerator = beatPerBar->intValue();
  result.timeSigDenominator = 4;
  result.timeInSamples = timeState.time;
  result.timeInSeconds = (double)(timeState.time)*sampleRate;
  result.editOriginTime = 0;

  result.isLooping=false;
  return true;
}
bool TimeManager::isAnyoneBoundToTime(){
  auto allListeners = timeManagerListeners.getListeners();
  for(auto &l:allListeners){
    if(l->isBoundToTime()){
      return true;
    }
  }
  return false;
}

void TimeManager::notifyListenerCleared(){

  if(isAnyoneBoundToTime()){
    return;
  }

  shouldRestart(false);
}


#if LINK_SUPPORT
void TimeManager::linkTempoCallBack(const double tempo){
  if(TimeManager * tm = getInstanceWithoutCreating()){
    tm->BPM->setValue(tempo);
  }
  
}
void TimeManager::linkNumPeersCallBack(const size_t numPeers){
  if(TimeManager * tm = getInstanceWithoutCreating()){
    tm->linkNumPeers->setValue((int)numPeers);
  }

}


#endif
