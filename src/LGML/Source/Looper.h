/*
  ==============================================================================

    Looper.h
    Created: 26 Apr 2016 4:46:37pm
    Author:  bkupe

  ==============================================================================
*/

#ifndef LOOPER_H_INCLUDED
#define LOOPER_H_INCLUDED

#include "ControllableContainer.h"
#include "LooperNode.h"

class LooperTrack;

#define MAX_NUM_TRACKS 32

class Looper : public NodeAudioProcessor, public ControllableContainer
{

public:
    Trigger * recPlaySelectedTrig;
    Trigger * playSelectedTrig;
    Trigger * clearSelectedTrig;
    Trigger * stopSelectedTrig;

    Trigger * selectAllTrig;
    Trigger * clearAllTrig;
    Trigger * stopAllTrig;

    FloatParameter * volumeSelected;
    BoolParameter * isMonitoring;
    IntParameter * numberOfTracks;

    OwnedArray<LooperTrack> tracks;

    LooperTrack * selectedTrack;


    Looper(LooperNode * looperNode);


    void setNumTracks(int numTracks);
    void addTrack();
    void removeTrack(int i);

    void selectMe(LooperTrack * t);


    bool askForBeingMasterTrack(LooperTrack * t);
    bool askForBeingAbleToPlayNow(LooperTrack *_t);
    bool areAllTrackClearedButThis(LooperTrack * _t);


    void onContainerTriggerTriggered(Trigger * t) override;
    void onContainerParameterChanged(Parameter * p) override;
    // internal
    void processBlockInternal(AudioBuffer<float>& buffer, MidiBuffer& midiMessages)override;
    void checkIfNeedGlobalLooperStateUpdate();


    LooperTrack * lastMasterTempoTrack;
    AudioBuffer<float> bufferIn;
    AudioBuffer<float>bufferOut;

    LooperNode * looperNode;


    //Listener
    class  Listener
    {
    public:

        /** Destructor. */
        virtual ~Listener() {}
        virtual void trackNumChanged(int num) = 0;
    };

    ListenerList<Listener> looperListeners;
    void addLooperListener(Listener* newListener) { looperListeners.add(newListener); }
    void removeLooperListener(Listener* listener) { looperListeners.remove(listener); }

};




#endif  // LOOPER_H_INCLUDED
