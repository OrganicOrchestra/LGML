/*
 ==============================================================================

 LooperTrack.h
 Created: 26 Apr 2016 4:11:41pm
 Author:  bkupe

 ==============================================================================
 */

#ifndef LOOPERTRACK_H_INCLUDED
#define LOOPERTRACK_H_INCLUDED

//#define BLOCKSIZEGRANULARITY
#include "ControllableContainer.h"

#include "PlayableBuffer.h"
#include "AudioHelpers.h"

class LooperNode;

#define MAX_LOOP_LENGTH_S 60

class LooperTrack : public ControllableContainer
{

public:
  LooperTrack(LooperNode * looper, int _trackIdx);
  ~LooperTrack() {}

  // only SHOULD value can be accessed from other thread than audio
  // then state is updated from audioThread
  enum TrackState {
    RECORDING,
    WILL_RECORD,
    PLAYING,
    WILL_PLAY,
    CLEARED,
    STOPPED,
    WILL_STOP
  };

  Trigger * selectTrig;
  Trigger * recPlayTrig;
  Trigger * playTrig;
  Trigger * clearTrig;
  Trigger * stopTrig;
  StringParameter  * stateParameterString;
  FloatParameter * volume;
  FloatParameter * beatLength;
  double startRecBeat,startPlayBeat;
  float logVolume;
  BoolParameter * mute;
  BoolParameter * solo;



  const float defaultVolumeValue = DB0_FOR_01;
  int trackIdx;

  static String trackStateToString(const TrackState & ts);
  void onContainerParameterChanged(Parameter * p) override;
  void onContainerTriggerTriggered(Trigger * t) override;

  //    Component * createDefaultUI(Component*)override;

  void processBlock(AudioBuffer<float>& buffer, MidiBuffer & midi);
  void setSelected(bool isSelected);


  void setTrackState(TrackState state);

  // from events like UI
  void askForSelection(bool isSelected);
  bool askForBeingMasterTempoTrack();
  bool isMasterTempoTrack();
  bool shouldWaitFirstOnset();
  bool hasOnset();
  float RMSIn;


  //Listener
  class  Listener : public AsyncUpdater
  {
  public:

    /** Destructor. */
    virtual ~Listener() {cancelPendingUpdate();}
    //                called from here
    void internalTrackStateChanged(const TrackState &state) {

      stateToBeNotified = state;
//      trackStateChanged(state);
      notifyStateChange = true;
      triggerAsyncUpdate();

    }

    void internalTrackTimeChanged(double position){
      int64 time = Time::getMillisecondCounter();
      trackPosition = position;
      if(time - lastTrackTime>trackUpdatePeriodMs){
      notifyTrackTime = true;
      triggerAsyncUpdate();
        lastTrackTime = time;
      }

    }

    void internalTrackSetSelected(bool t){
      isSelected = t;
      trackSelected(isSelected);
      notifySelectChange = true;
      triggerAsyncUpdate();
    }
    TrackState stateToBeNotified;
    bool notifyStateChange = false;

    bool isSelected;
    bool notifySelectChange = false;

    bool notifyTrackTime = false;
    double trackPosition=0;
    double trackUpdatePeriodMs = 100;
    int64 lastTrackTime=0;

    // dispatched to listeners
//    virtual void trackStateChanged(const TrackState &) {};
    virtual void trackStateChangedAsync(const TrackState & state) = 0;
    virtual void trackTimeChangedAsync(double position){};
    void setTrackTimeUpdateRateHz(float hz){
      trackUpdatePeriodMs = 1000.0/hz;
    }
    void handleAsyncUpdate() override {
      if(notifyStateChange){
        trackStateChangedAsync(stateToBeNotified);
        notifyStateChange = false;
      }
      if(notifySelectChange){
        trackSelectedAsync(isSelected);
        notifySelectChange = false;
      }
      if(notifyTrackTime){
        trackTimeChangedAsync(trackPosition);
      }

    }
    virtual void trackSelected(bool) {};
    virtual void trackSelectedAsync(bool) {};
  };

  ListenerList<Listener> trackStateListeners;
  void addTrackListener(Listener* newListener) { trackStateListeners.add(newListener); }
  void removeTrackListener(Listener* listener) { trackStateListeners.remove(listener); }


  class AsyncTrackStateStringSynchronizer : public LooperTrack::Listener {
  public:
    StringParameter * stringParameter;
    AsyncTrackStateStringSynchronizer(StringParameter  *origin) :stringParameter(origin) {}
    void trackStateChangedAsync(const TrackState &_trackState) override {
      stringParameter->setValue(trackStateToString(_trackState), false, true);
    }
  };
  ScopedPointer<AsyncTrackStateStringSynchronizer> stateParameterStringSynchronizer;


  TrackState trackState,desiredState;

  bool isSelected;

#if !LGML_UNIT_TESTS
private:
#endif

  friend class LooperNode;
  bool someOneIsSolo;

  uint64 quantizedRecordEnd, quantizedRecordStart;
  uint64 quantizedPlayStart, quantizedPlayEnd;

  bool updatePendingLooperTrackState(const uint64 curTime, int blockSize);
  void handleStartOfRecording();
  void handleEndOfRecording( );


  PlayableBuffer loopSample;
  float lastVolume;


  int getQuantization();



  double originBPM ;


  LooperNode * parentLooper;


  void cleanAllQuantizeNeedles();

  //friend class Looper;
};


#endif  // LOOPERTRACK_H_INCLUDED
