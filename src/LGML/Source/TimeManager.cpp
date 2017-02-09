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
firstPlayingFrame(false),
hasJumped(false),
hadMasterCandidate(false),
timeMasterCandidate(nullptr),
samplePerBeatGranularity(8)
{

  BPM = addFloatParameter("bpm","current BPM",120,(float)BPMRange.getStart(), (float)BPMRange.getEnd());
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

  clickFader = new FadeInOut(10000,10000,true,1.0/3.0);
}
TimeManager::~TimeManager()
{
}


void TimeManager::incrementClock(int time){
  jassert(blockSize!=0);
  if(time!=blockSize){
#if !LGML_UNIT_TESTS
    jassertfalse;
#endif
    setBlockSize(time);
  }
  updateState();
  if(_isLocked){
    
    return;
  }

  hasJumped = notifyTimeJumpedIfNeeded();


   if(!hasJumped && timeState.isPlaying){
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
  desiredTimeState =timeState;


  if(hadMasterCandidate && !isSettingTempo->boolValue()){
    timeMasterCandidate=nullptr;
  }
  hadMasterCandidate = timeMasterCandidate!=nullptr;

}


bool TimeManager::notifyTimeJumpedIfNeeded(){

if(timeState.isJumping){
  jassert(blockSize!=0);
  timeState.time=timeState.nextTime;
  timeState.nextTime = timeState.time+blockSize;
  timeState.isJumping=false;
  listeners.call(&Listener::timeJumped,timeState.time);
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

  if(click->boolValue()&&timeState.isPlaying){
    static uint64 sinCount = 0;

    bool isFirstBeat = (getClosestBeat()%beatPerBar->intValue()) == 0;
    const int sinFreq = sampleRate /(isFirstBeat?660:440);
    //    const int sinPeriod = sampleRate / sinFreq;
    const double k = 40.0;



    if(desiredTimeState.isJumping){
      clickFader->startFadeOut();
    }

    for(int i = 0 ; i < numSamples;i++){

      double carg = sinCount*1.0/sinFreq;

      double x = (getBeatInNextSamples(i)-getBeatInt() ) ;
      double h = k*fmod((double)x+1.0/k,1.0);
      clickFader->incrementFade();
      double env = clickFader->getCurrentFade()*jmax(0.0,h*exp(1.0-h));

      float res = (clickVolume->floatValue()* env* cos(2.0*M_PI*carg ));

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

  if(timeMasterCandidate==nullptr){
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
  else if(p==BPM){
    setBPMInternal(BPM->doubleValue(),true);
    clickFader->startFadeOut();

  }
  else if (p==BPMLocked){
    BPM->isEditable = !BPMLocked->boolValue();
  }
  else if(p==beatPerBar){currentBeat->maximumValue = beatPerBar->intValue();}


};

void TimeManager::shouldStop(){
  desiredTimeState.isPlaying = false;
  desiredTimeState.jumpTo(0);
}

void TimeManager::shouldRestart(bool playing){
  desiredTimeState.isPlaying=playing;
  desiredTimeState.jumpTo( 0);
}
void TimeManager::shouldGoToZero(){
  desiredTimeState.jumpTo(0);
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
void TimeManager::goToTime(uint64 time,bool now){
  desiredTimeState.jumpTo(time);
  if(now){
    updateState();
    notifyTimeJumpedIfNeeded();
  }
}


void TimeManager::shouldPlay(){
  desiredTimeState.isPlaying = true;
}

void TimeManager::updateState(){
  String dbg;
  if(timeState.isPlaying != desiredTimeState.isPlaying){
    dbg+="play:"+String(timeState.isPlaying)+"/"+String(desiredTimeState.isPlaying);
  }
  if(timeState.time != desiredTimeState.time){
    dbg+=" ::: time:"+String(timeState.time)+"/"+String(desiredTimeState.time);
  }
  if(desiredTimeState.isJumping){
    dbg+="time jumping to : " + String(desiredTimeState.nextTime);
  }
  if(dbg!=""){
    LOG(dbg);
  }

  firstPlayingFrame = desiredTimeState.isPlaying && timeState.isPlaying;

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
        currentBeatPeriod = (uint64)0.7; //@Martin 0.7 for a uint64 ??
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

        goToTime(targetBeatInt*currentBeatPeriod*44.1);//(deltaBeat*beatTimeInSample);

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
void TimeManager::setBPMInternal(double _BPM,bool adaptTimeInSample){
  isSettingTempo->setValue(false,false,false,true);
  int newBeatTime = (uint64)(sampleRate *1.0/ BPM->doubleValue()*60.0);

  if(adaptTimeInSample){
    uint64 targetTime = timeState.time*newBeatTime/beatTimeInSample;
    goToTime(targetTime);
  }
  beatTimeInSample =newBeatTime;

}
uint64 TimeManager::getTimeInSample(){
  return timeState.time;
}
uint64 TimeManager::getNextTimeInSample(){
  if(desiredTimeState.isJumping)return desiredTimeState.nextTime;
  else return  timeState.nextTime;
}
bool  TimeManager::willRestart(){
  return (timeState.nextTime!=0) && (desiredTimeState.nextTime==0);
}





TransportTimeInfo TimeManager::findTransportTimeInfoForLength(uint64 time){
  TransportTimeInfo res;
  res.sampleRate = sampleRate;
  res.barLength = 1;
  double time_seconds = time* 1.0/ sampleRate;
  res.beatTime = time_seconds* 1.0/beatPerBar->intValue();

  // over 150 bpm (0.4s)
  while(res.beatTime < beatTimeGuessRange.getStart()){res.beatTime*=2.0;res.barLength/=2.0;}
  // under 70 bpm (0.85s)
  while(res.beatTime > beatTimeGuessRange.getEnd() ){res.beatTime/=2.0;res.barLength*=2.0;}
  res.beatInSample = (res.beatTime*sampleRate);

  res.makeValidForGranularity(samplePerBeatGranularity);

  res.bpm = 60.0/res.beatTime;
  DBG("found beat Sample : " << String(res.beatInSample) << " : " << time);
  
  return res;
}
void TimeManager::setBPMFromTransportTimeInfo(const TransportTimeInfo & info,bool adaptTimeInSample){

  BPM->setValue(info.bpm,false,false,false);
  if(adaptTimeInSample){
  uint64 targetTime = timeState.time*info.beatInSample/beatTimeInSample;
  goToTime(targetTime);
  }
  // force exact beatTimeInSample
  beatTimeInSample = info.beatInSample;
  listeners.call(&Listener::BPMChanged,BPM->doubleValue());

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

  const uint64 samplesPerUnit = (beatTimeInSample*beatPerBar->intValue()/barFraction);
  const uint64 res = (uint64) ((floor(timeState.time*1.0/samplesPerUnit) + 1)*samplesPerUnit);
  return res;
}

uint64 TimeManager::getTimeForNextBeats(int beats){return (getBeatInt()+ beats)*beatTimeInSample;}

int     TimeManager::getBeatInt()   {return (int)floor(getBeat());}
double  TimeManager::getBeat()      {return (double)(timeState.time*1.0/beatTimeInSample);}
int     TimeManager::getClosestBeat(){return (int)floor(getBeat()+0.5);}
double  TimeManager::getBeatPercent() {return (double)(timeState.time*1.0/beatTimeInSample-getBeatInt());}
double  TimeManager::getBeatInNextSamples(int numSamplesToAdd){return(double) ((timeState.time+numSamplesToAdd)*1.0/beatTimeInSample);}


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

void TimeManager::notifyListenerCleared(){
  auto allListeners = listeners.getListeners();
  for(auto &l:allListeners){
    if(l->isBoundToTime()){
      return;
    }
  }


  shouldRestart(false);
}
