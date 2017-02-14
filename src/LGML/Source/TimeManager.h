/*
 ==============================================================================

 TimeManager.h
 Created: 2 Mar 2016 8:33:44pm
 Author:  bkupe

 ==============================================================================
 */

#ifndef TIMEMANAGER_H_INCLUDED
#define TIMEMANAGER_H_INCLUDED
#pragma once


/*
 This singleton handle time at sample Level
 then can dispatch synchronous or asynchronous event via TimeManager::Listener
 Also provide basic click ability, and tap tempo via its parameters click and tapTempo
 */

#include "TimeMasterCandidate.h"
#include "ControllableContainer.h"

// TODO change when window support
// @ ben header only so should be easy
#if JUCE_MAC
#define LINK_SUPPORT 1
#else
#define LINK_SUPPORT 0
#endif


#if LINK_SUPPORT
#include "ableton/link.hpp"
#include "ableton/link/HostTimeFilter.hpp"
#else

#endif

class FadeInOut;


struct TransportTimeInfo{
  double barLength;
  double beatTime;
  int beatInSample;
  double sampleRate;
  double bpm;



  void makeValidForGranularity(int samplePerBeatGranularity){
    if(samplePerBeatGranularity>0){
      int offset =((int)beatInSample)%samplePerBeatGranularity;
      if(offset>samplePerBeatGranularity/2){offset = -(samplePerBeatGranularity-offset);}
      beatInSample = beatInSample - offset;
      beatTime = beatInSample*1.0/sampleRate;

    };
  }
  
  

};


class TimeManager : public AudioIODeviceCallback ,public ControllableContainer,public AudioPlayHead,
public TimeMasterCandidate
{


  public :

  // TODO check if we can use SingleThread Singleton for fast access in processAdio
  juce_DeclareSingleton(TimeManager, true);

  TimeManager();
  ~TimeManager();


  BoolParameter * playState;
  Trigger * playTrigger;
  Trigger * stopTrigger;
  Trigger * tapTempo;
  BoolParameter * isSettingTempo;
  FloatParameter *  BPM;
  IntParameter * currentBeat;
  IntParameter * currentBar;
  IntParameter * beatPerBar;
  BoolParameter * BPMLocked;
  BoolParameter * click;
  FloatParameter * clickVolume;
  BoolParameter * isWaitingForStart;

  IntParameter * quantizedBarFraction;

  #if LINK_SUPPORT
  ableton::Link linkSession;
  
  static void linkNumPeersCallBack(const std::size_t numPeers);
  static void linkTempoCallBack(const double tempo);
  bool getLinkTimeLine();
  #endif
  BoolParameter * linkEnabled;
  IntParameter * linkNumPeers;



  void setSampleRate(int sr);
  void setBlockSize(int bS);




  TransportTimeInfo findTransportTimeInfoForLength(uint64 time);
  void setBPMFromTransportTimeInfo(const TransportTimeInfo & info,bool adaptTime);


  void jump(int amount);
  void goToTime(uint64 time,bool now = false);
  void advanceTime(uint64 ,bool now = false);

  // used when triggering multiple change
  void lockTime(bool );
  bool isLocked();

  void play(bool shouldPlay);
  void togglePlay();

  int getBeatInt();
  double getBeat();
  uint64 getNextGlobalQuantifiedTime();
  uint64 getNextQuantifiedTime(int barFraction);
  uint64 getTimeForNextBeats(int beats);
  uint64 getTimeInSample();
  uint64 getNextTimeInSample();
  bool willRestart();
  int getClosestBeat();
  double getBeatInNextSamples(int numSampleToAdd);
  double getBeatForQuantum(const double q);


  bool isPlaying();
  bool isFirstPlayingFrame();
  bool isJumping();

  //return percent in beat
  double getBeatPercent();
  int getBar();



  uint64 beatTimeInSample;
  int sampleRate;
  int blockSize;

  // instance currently handling tempo (loop track while recording)
  TimeMasterCandidate *  timeMasterCandidate;
  bool isMasterCandidate(TimeMasterCandidate * n);
  bool hasMasterCandidate();
  void releaseMasterCandidate(TimeMasterCandidate * n);
  void releaseIfMasterCandidate(TimeMasterCandidate * n);
  bool askForBeingMasterCandidate(TimeMasterCandidate * n);
  void audioDeviceIOCallback (const float** inputChannelData,int numInputChannels,float** outputChannelData,int numOutputChannels,int numSamples) override;

  bool hadMasterCandidate;


  bool getCurrentPosition (CurrentPositionInfo& result)override;

  void notifyListenerCleared();

  class Listener{
  public:
    virtual ~Listener(){};
    virtual void BPMChanged(double BPM){};
    virtual void timeJumped(uint64 time) {};
    virtual void playStop(bool playStop){};


    // info for stopping manager if needed;
    virtual bool isBoundToTime()=0;



  };

  ListenerList<Listener> listeners;


   void addTimeManagerListener(Listener* newListener) { listeners.add(newListener); }
   void removeTimeManagerListener(Listener* listener) { listeners.remove(listener); }


#if !LGML_UNIT_TESTS
private:
#endif

  struct TimeState{
    TimeState():isJumping(false),nextTime((uint64)-1),isPlaying(false),time((uint64)0){}
    bool isPlaying;
    void jumpTo(uint64 t){
      isJumping = true;
      nextTime = t;
    }
    bool isJumping;
    uint64 nextTime;
    uint64 time;
  };

  TimeState timeState,desiredTimeState;
  long long audioClock;

  void shouldStop();
  void shouldPlay();
  void shouldRestart(bool );
  void shouldGoToZero();

  void updateState();
  void incrementClock(int block);

  void onContainerParameterChanged(Parameter * )override;
  void onContainerTriggerTriggered(Trigger * ) override;


  void setBPMInternal(double BPM,bool adaptTime);

  virtual void audioDeviceAboutToStart (AudioIODevice* device)override {
    setSampleRate((int)device->getCurrentSampleRate());
    setBlockSize((int)device->getCurrentBufferSizeSamples());
    // should we notify blockSize?
  };

  /** Called to indicate that the device has stopped. */
  virtual void audioDeviceStopped() override{

  };
  bool _isLocked;
  void updateCurrentPositionInfo();

  CurrentPositionInfo currentPositionInfo;

  // used for guessing tempo
  Range<double> beatTimeGuessRange;
  // used for manual setting of tempo
  Range<double> BPMRange;


  // granularity ensure that beat sample is divisible by 8 (8,4,2 ... 1) for further sub quantifs
  const int samplePerBeatGranularity;
  

  uint64 lastTaped;
  uint64 currentBeatPeriod;
  int tapInRow;

  bool hasJumped;

  ScopedPointer<FadeInOut> clickFader;
  bool notifyTimeJumpedIfNeeded();

  bool isAnyoneBoundToTime();

  void checkCommitableParams();
  #if LINK_SUPPORT
  ableton::Link::Timeline  linkTimeLine;
  std::chrono::microseconds  linkTime;
  ableton::link::HostTimeFilter<ableton::link::platform::Clock> linkFilter;
  std::chrono::microseconds linkLatency;
  #endif


  void pushCommitableParams();
//  double lastEnv;
//  int clickFadeOut,clickFadeIn,clickFadeTime;
  JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(TimeManager)

};



#endif  // TIMEMANAGER_H_INCLUDED
