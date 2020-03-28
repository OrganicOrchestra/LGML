/* Copyright © Organic Orchestra, 2017
*
* This file is part of LGML.  LGML is a software to manipulate sound in realtime
*
* This program is free software; you can redistribute it and/or modify
* it under the terms of the GNU General Public License as published by
* the Free Software Foundation (version 3 of the License).
*
* This program is distributed in the hope that it will be useful,
* but WITHOUT ANY WARRANTY; without even the implied warranty of
* MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
*
*/


#ifndef LOOPERTRACK_H_INCLUDED
#define LOOPERTRACK_H_INCLUDED


#include "../../Controllable/Parameter/ParameterContainer.h"

#include "../../Audio/PlayableBuffer.h"
#include "../../Audio/AudioHelpers.h"

class LooperNode;



class LooperTrack : public ParameterContainer, public EnumParameter::EnumListener
{

public:
    LooperTrack (LooperNode* looper, int _trackIdx);
    ~LooperTrack() ;

    //   value SHOULD only be accessed from other thread than audio
    // then state is updated from audioThread
    enum TrackState
    {
        RECORDING,
        WILL_RECORD,
        PLAYING,
        WILL_PLAY,
        CLEARED,
        STOPPED,
        WILL_STOP
    };

    Trigger* selectTrig;
    Trigger* recPlayTrig;
    Trigger* recStopTrig;
    Trigger* playTrig;
    Trigger* clearTrig;
    Trigger* stopTrig;
    Trigger* togglePlayStopTrig;
    EnumParameter* sampleChoice;

    StringParameter*   stateParameterString;
    FloatParameter* volume;
    FloatParameter* beatLength;
    double startRecBeat, startPlayBeat;
    SimpleSmoothedValue<float> logVolume;
    BoolParameter* mute;
    BoolParameter* solo;
    FloatParameter*   originBPM ;



    void clear();
    void stop();
    void play();
    void recOrStop();
    void recPlay();
    bool isBusy();
    bool needAudioIn();
    Array<float> getNormalizedOnsets();



    int trackIdx;

    static String trackStateToString (const TrackState& ts);
    void onContainerParameterChanged ( ParameterBase* p) override;
    void onContainerTriggerTriggered (Trigger* t) override;

    //    Component * createDefaultUI(Component*)override;

    void processBlock (AudioBuffer<float>& buffer, MidiBuffer& midi);
    void setSelected (bool isSelected);

    bool isEmpty();

    void setTrackState (TrackState state);

    // from events like UI
    void askForSelection (bool isSelected);
    bool askForBeingMasterTempoTrack();
    bool isMasterTempoTrack();
    bool shouldWaitFirstOnset();
    bool hasOnset();
    float RMSIn;



    //Listener
    class  Listener : private AsyncUpdater
    {
    public:
        Listener()
        {
            notifyStateChange = false;
            notifySelectChange = false ;


        }
        /** Destructor. */
        virtual ~Listener() {cancelPendingUpdate();}


        void internalTrackStateChanged (const TrackState& state)
        {
            stateToBeNotified = state;
            notifyStateChange = true;
            triggerAsyncUpdate();
        }



        void internalTrackSetSelected (bool t)
        {
            isSelected = t;
            trackSelected (isSelected);
            notifySelectChange = true;
            triggerAsyncUpdate();
        }

        TrackState stateToBeNotified;
        Atomic<int> notifyStateChange ;

        bool isSelected;
        Atomic<int> notifySelectChange ;


        // dispatched to listeners
        //    virtual void trackStateChanged(const TrackState &) {}
        virtual void trackStateChangedAsync (const TrackState& state) = 0;

        void handleAsyncUpdate() override
        {
            if (notifyStateChange.compareAndSetBool (0, 1))
            {
                trackStateChangedAsync (stateToBeNotified);

            }

            if (notifySelectChange.compareAndSetBool (0, 1))
            {
                trackSelectedAsync (isSelected);
            }

        }
        virtual void trackSelected (bool) {}
        virtual void trackSelectedAsync (bool) {}
    };

    class TrackTimeListener: public Listener{
    public:
        TrackTimeListener():tsUpdater(this){
            notifyTrackTime = false;
        }
        void internalTrackTimeChanged (double position)
        {
            int64 time = Time::getMillisecondCounter();
            trackPosition = position;

            if (time - lastTrackTime > trackUpdatePeriodMs)
            {
                notifyTrackTime = true;
                tsUpdater.triggerAsyncUpdate();
                lastTrackTime = time;
            }

        }
        virtual void trackTimeChangedAsync (double /*position*/) {}
        void setTrackTimeUpdateRateHz (float hz){trackUpdatePeriodMs = 1000.0 / hz;}

        struct TimeAsyncUpdater : public juce::AsyncUpdater{
            TimeAsyncUpdater(TrackTimeListener * tl):owner(tl){}
            void handleAsyncUpdate() override{owner->trackTimeChangedAsync(owner->trackPosition);}
            TrackTimeListener * owner;
        };

        TimeAsyncUpdater tsUpdater;
        Atomic<int> notifyTrackTime;
        double trackPosition = 0;
        double trackUpdatePeriodMs = 100;
        int64 lastTrackTime = 0;

    };
    ListenerList<Listener> trackStateListeners;
    ListenerList<TrackTimeListener> trackTimeListeners;
    void addTrackListener (Listener* newListener) { trackStateListeners.add (newListener); }
    void removeTrackListener (Listener* listener) { trackStateListeners.remove (listener); }
    void addTrackListener (TrackTimeListener* newListener) { trackTimeListeners.add(newListener);trackStateListeners.add (newListener); }
    void removeTrackListener (TrackTimeListener* listener) { trackTimeListeners.remove(listener);trackStateListeners.remove (listener); }

    class AsyncTrackStateStringSynchronizer : public LooperTrack::Listener
    {
    public:
        StringParameter* stringParameter;
        AsyncTrackStateStringSynchronizer (StringParameter*  origin) : stringParameter (origin) {}
        void trackStateChangedAsync (const TrackState& _trackState) override
        {
            stringParameter->setValue (trackStateToString (_trackState), false, true);
        }
    };
    std::unique_ptr<AsyncTrackStateStringSynchronizer> stateParameterStringSynchronizer;


    TrackState trackState, desiredState;

    bool isSelected;


    void setNumChannels (int numChannels);
#if !LGML_UNIT_TESTS
private:
#endif
    
    friend class LooperNode;
    bool someOneIsSolo,shouldHaveZeroGain;

    sample_clk_t quantizedRecordEnd, quantizedRecordStart;
    sample_clk_t quantizedPlayStart, quantizedPlayEnd;

    bool updatePendingLooperTrackState (  int blockSize);
    void handleStartOfRecording();
    void handleEndOfRecording( );


    PlayableBuffer playableBuffer;
    float lastVolume;


    int getQuantization();






    LooperNode* parentLooper;

    void releaseMasterTrack();
    void cleanAllQuantizeNeedles();

    
    void enumOptionSelectionChanged (EnumParameter*, bool isSelected, bool isValid, const Identifier&)override;


    bool loadAudioSample (const String& file);
    bool isLoadingAudioFile;
    bool hadOnset;
    
};


#endif  // LOOPERTRACK_H_INCLUDED
