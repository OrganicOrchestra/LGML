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

    //Listener
    class  Listener : public AsyncUpdater
    {
    public:

        /** Destructor. */
        virtual ~Listener() {cancelPendingUpdate();}
        //                called from here
        void internalTrackStateChanged(const TrackState &state) {
            notifyStateChange = true;
            stateToBeNotified = state;
            trackStateChanged(state);
            triggerAsyncUpdate();
            
        }
        
        void internalTrackSetSelected(bool t){
            notifySelectChange = true;
            isSelected = t;
            trackSelected(isSelected);
            triggerAsyncUpdate();
        }
        TrackState stateToBeNotified;
        bool notifyStateChange = false;
        
        bool isSelected;
        bool notifySelectChange = false;

        // dispatched to listeners
        virtual void trackStateChanged(const TrackState &) {};
        virtual void trackStateChangedAsync(const TrackState & state) = 0;
        void handleAsyncUpdate() override {
            if(notifyStateChange){
            trackStateChangedAsync(stateToBeNotified);
                notifyStateChange = false;
            }
            if(notifySelectChange){
                trackSelectedAsync(isSelected);
                notifySelectChange = false;
            }
        
        }
        virtual void trackSelected(bool) {};
        virtual void trackSelectedAsync(bool) {};
    };

    ListenerList<Listener> trackStateListeners;
    void addTrackListener(Listener* newListener) { trackStateListeners.add(newListener); }
    void removeTrackListener(Listener* listener) { trackStateListeners.remove(listener); }


    class AsyncTrackStateStringSynchroizer : public LooperTrack::Listener {
    public:
        StringParameter * stringParameter;
        AsyncTrackStateStringSynchroizer(StringParameter  *origin) :stringParameter(origin) {}
        void trackStateChangedAsync(const TrackState &_trackState) override {
            stringParameter->setValue(trackStateToString(_trackState), false, true);
        }
    };
    ScopedPointer<AsyncTrackStateStringSynchroizer> stateParameterStringSynchronizer;


     TrackState trackState,desiredState;

    bool isSelected;
    
private:

    friend class LooperNode;
    bool someOneIsSolo;

    Atomic<uint64> quantizedRecordEnd, quantizedRecordStart;
    Atomic<uint64> quantizedPlayStart, quantizedPlayEnd;

    bool updatePendingLooperTrackState(const uint64 curTime, int blockSize);
    void padBufferIfNeeded(int granularity = 0);
  void fillBufferIfNeeded();

    PlayableBuffer loopSample;
    float lastVolume;
    bool isFadingIn;
    bool isCrossFading;


  int getQuantization();



    double originBPM ;


    LooperNode * parentLooper;


    void cleanAllQuantizeNeedles();
    //friend class Looper;
};


#endif  // LOOPERTRACK_H_INCLUDED
