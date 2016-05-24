/*
  ==============================================================================

    LooperTrack.h
    Created: 26 Apr 2016 4:11:41pm
    Author:  bkupe

  ==============================================================================
*/

#ifndef LOOPERTRACK_H_INCLUDED
#define LOOPERTRACK_H_INCLUDED

#include "AudioHelpers.h"
#include "ControllableContainer.h"

class LooperNode;

#define MAX_LOOP_LENGTH_S 30

class LooperTrack : public ControllableContainer
{

public:
    LooperTrack(LooperNode * looper, int _trackIdx);
    ~LooperTrack() {}

    // only SHOULD value can be accessed from other thread than audio
    // then state is updated from audioThread
    enum TrackState {
        SHOULD_RECORD = 0,
        SHOULD_PLAY,
        SHOULD_CLEAR,
        SHOULD_STOP,

        RECORDING,
        PLAYING,
        CLEARED,
        STOPPED
    };

    Trigger * selectTrig;
    Trigger * recPlayTrig;
    Trigger * playTrig;
    Trigger * clearTrig;
    Trigger * stopTrig;
    StringParameter  * stateParameterString;
    FloatParameter * volume;
    IntParameter * beatLength;
    int startBeat;
    float logVolume;
    BoolParameter * mute;
    BoolParameter * solo;

    TrackState trackState;

    bool someOneIsSolo;

    const float defaultVolumeValue = 0.8f;
    int trackIdx;

    static String trackStateToString(const TrackState & ts);
    void onContainerParameterChanged(Parameter * p) override;
    void onContainerTriggerTriggered(Trigger * t) override;

//    Component * createDefaultUI(Component*)override;

    void processBlock(AudioBuffer<float>& buffer, MidiBuffer & midi);
    void setSelected(bool isSelected);


    void setTrackState(TrackState state,int quantification = -1);

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
            stateToBeNotified = state;

            trackStateChanged(state);
            triggerAsyncUpdate();
        }
        TrackState stateToBeNotified;

        // dispatched to listeners
        virtual void trackStateChanged(const TrackState &) {};
        virtual void trackStateChangedAsync(const TrackState & state) = 0;
        void handleAsyncUpdate() override { trackStateChangedAsync(stateToBeNotified); }
        virtual void trackSelected(bool) {};
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


    enum InternalTrackState {
        BUFFER_STOPPED = 0,
        BUFFER_PLAYING,
        BUFFER_RECORDING

    };


    InternalTrackState internalTrackState;
    InternalTrackState lastInternalTrackState;

    int recordNeedle;
    int quantizedRecordEnd, quantizedRecordStart;

    int playNeedle;
    int quantizedPlayStart, quantizedPlayEnd;

    void updatePendingLooperTrackState(const uint64 curTime, int blockSize);


    AudioSampleBuffer loopSample;
    float lastVolume;
    // represent audioProcessor behaviour



    // keeps track of few bits of audio
    // to readjust the loop when controllers are delayed
    RingBuffer streamAudioBuffer;
    IntParameter * preDelayMs;

    LooperNode * parentLooper;


    void cleanAllQuantizeNeedles();
    //friend class Looper;
};


#endif  // LOOPERTRACK_H_INCLUDED
