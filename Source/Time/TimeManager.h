/*
 ==============================================================================

 Copyright © Organic Orchestra, 2017

 This file is part of LGML. LGML is a software to manipulate sound in realtime

 This program is free software; you can redistribute it and/or modify
 it under the terms of the GNU General Public License as published by
 the Free Software Foundation (version 3 of the License).

 This program is distributed in the hope that it will be useful,
 but WITHOUT ANY WARRANTY; without even the implied warranty of
 MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.

 ==============================================================================
 */

#ifndef TIMEMANAGER_H_INCLUDED
#define TIMEMANAGER_H_INCLUDED
#pragma once


/*
 This singleton handle time at sample Level
 then can dispatch synchronous or asynchronous event via TimeManager::TimeManagerListener
 Also provide basic click ability, and tap tempo via its parameters click and tapTempo
 */

#include "TimeMasterCandidate.h"
#include "../Controllable/Parameter/ParameterContainer.h"
#include "../Audio/AudioHelpers.h"
#include "../Audio/AudioConfig.h"



class LinkImpl;

class FadeInOut;


struct TransportTimeInfo
{
    double barLength;
    double beatTime;
    int beatInSample;
    double sampleRate;
    double bpm;



    void makeValidForGranularity (int samplePerBeatGranularity)
    {
        if (samplePerBeatGranularity > 0)
        {
            int offset = ((int)beatInSample) % samplePerBeatGranularity;

            if (offset > samplePerBeatGranularity / 2) {offset = - (samplePerBeatGranularity - offset);}

            beatInSample = beatInSample - offset;
            beatTime = beatInSample * 1.0 / sampleRate;

        };
    }



};


class TimeManager : public AudioIODeviceCallback, public ParameterContainer, public AudioPlayHead,
    public TimeMasterCandidate
{


public :

    // TODO check if we can use SingleThread Singleton for fast access in processAdio
    juce_DeclareSingleton (TimeManager, true);

    TimeManager();
    ~TimeManager();


    BoolParameter* playState;
    Trigger* playTrigger;
    Trigger* stopTrigger;
    Trigger* tapTempo;
    BoolParameter* isSettingTempo;
    FloatParameter*   BPM;
    IntParameter* currentBeat;
    IntParameter* currentBar;
    IntParameter* beatPerBar;
    BoolParameter* BPMLocked;
    BoolParameter* click;
    FloatParameter* clickVolume;




    IntParameter* quantizedBarFraction;

    ScopedPointer<LinkImpl> linkPimpl;
    friend class LinkPimpl;
    FloatParameter* linkLatencyParam;

    BoolParameter* linkEnabled;
    IntParameter* linkNumPeers;




    void setSampleRate (int sr);
    void setBlockSize (int bS);




    TransportTimeInfo findTransportTimeInfoForLength (sample_clk_t time, double sampleRate = -1);
    void setBPMFromTransportTimeInfo (const TransportTimeInfo& info, bool adaptTime, sample_clk_t atSample);


    void jump (int amount);
    void goToTime (sample_clk_t time, bool now = false);
    void advanceTime (sample_clk_t, bool now = false);

    // used when triggering multiple change
    void lockTime (bool );
    bool isLocked();

    void play (bool shouldPlay);
    void togglePlay();

    int getBeatInt();
    double getBeat();
    sample_clk_t getNextGlobalQuantifiedTime();
    sample_clk_t getNextQuantifiedTime (int barFraction);
    sample_clk_t getTimeForNextBeats (int beats);
    sample_clk_t getTimeInSample();
    sample_clk_t getNextTimeInSample();
    bool willRestart();
    int getClosestBeat();
    double getBeatInNextSamples (int numSampleToAdd);
    double getBeatForQuantum (const double q);


    bool isPlaying();
    bool isFirstPlayingFrame();
    bool isJumping();

    //return percent in beat
    double getBeatPercent();
    int getBar();



    sample_clk_t beatTimeInSample;
    int sampleRate;
    int blockSize;

    // instance currently handling tempo (loop track while recording)
    TimeMasterCandidate*   timeMasterCandidate;
    bool isMasterCandidate (TimeMasterCandidate* n);
    bool hasMasterCandidate();
    void releaseMasterCandidate (TimeMasterCandidate* n);
    void releaseIfMasterCandidate (TimeMasterCandidate* n);
    bool askForBeingMasterCandidate (TimeMasterCandidate* n);
    void audioDeviceIOCallback (const float** inputChannelData, int numInputChannels, float** outputChannelData, int numOutputChannels, int numSamples) override;

    bool hadMasterCandidate;


    bool getCurrentPosition (CurrentPositionInfo& result)override;

    void notifyListenerCleared();

    class TimeManagerListener
    {
    public:
        virtual ~TimeManagerListener() {};
        virtual void BPMChanged (double /*BPM*/) {};
        virtual void timeJumped (sample_clk_t /*time*/) {};
        virtual void playStop (bool /*playStop*/) {};
        // info for stopping manager if needed;
        virtual bool isBoundToTime() = 0;



    };

    ListenerList<TimeManagerListener> timeManagerListeners;

    void addTimeManagerListener (TimeManagerListener* newListener) { timeManagerListeners.add (newListener); }
    void removeTimeManagerListener (TimeManagerListener* listener) { timeManagerListeners.remove (listener); }


#if !LGML_UNIT_TESTS
private:
#endif

    struct TimeState
    {
        TimeState(): isJumping (false), nextTime (0), isPlaying (false), time (0) {}
        bool isPlaying;
        void jumpTo (sample_clk_t t)
        {
            isJumping = true;
            nextTime = t;
        }
        bool isJumping;
        sample_clk_t nextTime;
        sample_clk_t time;
    };

    TimeState timeState, desiredTimeState;
    sample_clk_t audioClock;

    void shouldStop (bool now = false);
    void shouldPlay (bool now = false);
    void shouldRestart (bool );
    void shouldGoToZero();

    void updateState();
    void incrementClock (int block);

    void onContainerParameterChanged ( ParameterBase* )override;
    void onContainerTriggerTriggered (Trigger* ) override;


    void setBPMInternal (double BPM, bool adaptTime);

    virtual void audioDeviceAboutToStart (AudioIODevice* device)override
    {
        setSampleRate ((int)device->getCurrentSampleRate());
        setBlockSize ((int)device->getCurrentBufferSizeSamples());
        // should we notify blockSize?
    };

    /** Called to indicate that the device has stopped. */
    virtual void audioDeviceStopped() override
    {

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


    sample_clk_t lastTaped;
    sample_clk_t currentBeatPeriod;
    int tapInRow;

    bool hasJumped;

    ScopedPointer<FadeInOut> clickFader;
    bool updateAndNotifyTimeJumpedIfNeeded();

    bool isAnyoneBoundToTime();

    void checkCommitableParams();



    void pushCommitableParams();
    //  double lastEnv;
    //  int clickFadeOut,clickFadeIn,clickFadeTime;
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (TimeManager)

};



#endif  // TIMEMANAGER_H_INCLUDED
