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
ControllableContainer("time"),
beatTimeGuessRange(.4,.85),
BPMRange(10,600),
_isLocked(false),
settingTempoFromCandidate(false),
currentBeatPeriod(.5),
lastTaped(0),
tapInRow(0),
firstPlayingFrame(false),
hasJumped(false)
{

  BPM = addFloatParameter("bpm","current BPM",120,(float)BPMRange.getStart(), (float)BPMRange.getEnd());
  playState = addBoolParameter("Play_Stop", "play or stop global transport", false);
  BPMLocked = addBoolParameter("bpmLocked", "bpm is locked by somebody", false);
  BPMLocked->isControllableFeedbackOnly = true;
  isSettingTempo = addBoolParameter("isSettingTempo", "is someone setting tempo (recording first loop)", false);
  currentBar  = addIntParameter("currentBar", "currentBar in transport", 0, 0, 9999999);
  currentBeat  = addIntParameter("currentBeat", "currentBeat in transport", 0, 0, 999999);
  beatPerBar = addIntParameter("beatPerBar", "beat Per Bar", 4, 1, 8);
  playTrigger = addTrigger("play", "trigger play");
  stopTrigger = addTrigger("stop", "trigger stop");
  quantizedBarFraction = addIntParameter("globalQuantization", "Global quantization in fraction of a bar", 1, 0, 16);
  tapTempo = addTrigger("tapTempo", "tap the tempo");
  click = addBoolParameter("metronome", "metronome", false);
  setBPMInternal(BPM->doubleValue());

  clickFader = new FadeInOut(10000,10000,true,1.0/3.0);
}
TimeManager::~TimeManager()
{
}


void TimeManager::incrementClock(int time){
  updateState();
  if(_isLocked)return;

  hasJumped = timeState.isJumping;
  if(timeState.isJumping){
    timeState.time=timeState.nextTime;
    timeState.isJumping=false;
  }

  else if(timeState.isPlaying){
    timeState.time+=time;
  }
  timeState.nextTime = timeState.time+time;
  int lastBeat = getBeatInt();
  int newBeat = getBeatInt();
  if(lastBeat!=newBeat){
    currentBeat->setValue(newBeat);
    if(newBeat%((int)beatPerBar->value) == 0){
      currentBar->setValue(getBar());

    }
  }
  desiredTimeState =timeState;

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

      float res = (env* cos(2.0*M_PI*carg ));

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
  potentialTimeMasterCandidate.addIfNotAlreadyThere(n);
  return potentialTimeMasterCandidate.getUnchecked(0) == n;
}

bool TimeManager::isMasterCandidate(TimeMasterCandidate * n){
  return potentialTimeMasterCandidate.size()>0 && n==potentialTimeMasterCandidate.getUnchecked(0);
}
bool TimeManager::hasMasterCandidate(){
  return potentialTimeMasterCandidate.size()>0;
}
void TimeManager::releaseMasterCandidate(TimeMasterCandidate * n){
  potentialTimeMasterCandidate.removeFirstMatchingValue(n);
  if(potentialTimeMasterCandidate.size()==0||
     (potentialTimeMasterCandidate.size()==1 && potentialTimeMasterCandidate.getUnchecked(0)==this)  ){
    stopTrigger->trigger();
    BPMLocked->setValue(false);

  }
}

void TimeManager::onContainerParameterChanged(Parameter * p){
  if(p==playState){
    if(!playState->boolValue()){
      if(isMasterCandidate(this)){potentialTimeMasterCandidate.clear();}
      shouldStop();
      clickFader->startFadeOut();
    }
    else{
      if (!hasMasterCandidate()) {askForBeingMasterCandidate(this);}
      shouldGoToZero();
      shouldPlay();
      clickFader->startFadeIn();
    }
  }
  else if(p==BPM){
    setBPMInternal(BPM->doubleValue());
    clickFader->startFadeOut();

  }
  else if (p==BPMLocked){
    if(BPMLocked->boolValue()){BPM->setRange(BPM->doubleValue(),BPM->doubleValue());}
    else                      {BPM->setRange(BPMRange.getStart(),BPMRange.getEnd());}
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
void TimeManager::advanceTime(uint64 a){

  if(desiredTimeState.isJumping){desiredTimeState.nextTime+=a;}
  else                          {desiredTimeState.jumpTo(timeState.nextTime+a);}

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
    playState->setValue(false);
    playState->setValue(true);
  }
  else if(t==stopTrigger){
    playState->setValue(false);
    isSettingTempo->setValue(false);
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

void TimeManager::setBPMInternal(double ){
  isSettingTempo->setValue(false);
  beatTimeInSample =(uint64)(sampleRate *1.0/ BPM->doubleValue()*60.0);
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

void TimeManager::jump(int amount){
  goToTime(timeState.time+amount);


}
void TimeManager::goToTime(uint64 time){
  desiredTimeState.nextTime = time;
  desiredTimeState.isJumping = true;
}



double TimeManager::setBPMForLoopLength(uint64 time,int granularity){
  double barLength = 1;
  double time_seconds = time* 1.0/ sampleRate;
  double beatTime = time_seconds* 1.0/beatPerBar->intValue();

  // over 150 bpm (0.4s)
  while(beatTime < beatTimeGuessRange.getStart()){beatTime*=2.0;barLength/=2.0;}
  // under 70 bpm (0.85s)
  while(beatTime > beatTimeGuessRange.getEnd() ){beatTime/=2.0;barLength*=2.0;}
  int beatInSample = (int)(beatTime*sampleRate);
  if(granularity>0){
    int offset =beatInSample%granularity;
    if(offset>granularity/2){offset = -(granularity-offset);}
    beatInSample = beatInSample - offset;
    beatTime = beatInSample*1.0/sampleRate;
  }
  settingTempoFromCandidate = true;
  BPM->setValue(60.0/beatTime);
  settingTempoFromCandidate = false;
  // force exact beatTimeInSample
  beatTimeInSample = beatInSample;
  DBG("beat Sample : " << String(beatTimeInSample) << " : " << time);
  shouldGoToZero();
  //jassert((int)(barLength*beatPerBar->intValue())>0);

  // lockBPM for now
  BPMLocked->setValue(true);

  return (barLength*beatPerBar->intValue());
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
  //TODO: check
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
