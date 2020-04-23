/*
 ==============================================================================

 Copyright © Organic Orchestra, 2017

 This file is part of LGML. LGML is a software to manipulate sound in real-time

 This program is free software; you can redistribute it and/or modify
 it under the terms of the GNU General Public License as published by
 the Free Software Foundation (version 3 of the License).

 This program is distributed in the hope that it will be useful,
 but WITHOUT ANY WARRANTY; without even the implied warranty of
 MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.

 ==============================================================================
 */

#ifndef _USE_MATH_DEFINES
    #define _USE_MATH_DEFINES
    #include <cmath>
#endif


#include "TimeManager.h"


#if (defined JUCE_MAC || defined JUCE_LINUX || defined JUCE_WINDOWS)
    #define LINK_SUPPORT 1
#else
    #define LINK_SUPPORT 0
#endif

#if JUCE_DBG
#define DBGTIME(x) //DBG(x)
#else
#define DBGTIME(x)
#endif
juce_ImplementSingleton (TimeManager);

#include "../Node/NodeBase.h"
#include "../Utils/DebugHelpers.h"
#include "../Audio/AudioHelpers.h"

extern AudioDeviceManager& getAudioDeviceManager();

#include "LinkImpl.hpp"
#include "../MIDI/MIDIClock.h"

TimeManager::TimeManager():
    beatTimeInSample (1000),
    sampleRate (44100),
    blockSize (0),
    ParameterContainer ("Time"),
    beatTimeGuessRange (.4, .85),
    BPMRange (40, 250),
    _isLocked (false),
    currentBeatPeriod (0),
    lastTaped (0),
    tapInRow (0),
    hasJumped (false),
    hadMasterCandidate (false),
    timeMasterCandidate (nullptr),
    samplePerBeatGranularity (8),
    audioClock (0)
{
    nameParam->setInternalOnlyFlags(true,false);

    BPM = addNewParameter<FloatParameter> ("BPM", "current BPM", 120.f, (float)BPMRange.getStart(), (float)BPMRange.getEnd());
    BPM->isCommitableParameter = true;
    playState = addNewParameter<BoolParameter> ("Play Stop", "play or stop global transport", false);
    playState->isSavable=false;
    BPMLocked = addNewParameter<BoolParameter> ("BPM Locked", "bpm is locked by somebody", false);
    BPMLocked->setInternalOnlyFlags(true,false);
    isSettingTempo = addNewParameter<BoolParameter> ("Is Setting Tempo", "is someone setting tempo (recording first loop)", false);
    isSettingTempo->setInternalOnlyFlags(true,false);
    currentBar  = addNewParameter<IntParameter> ("Current Bar", "currentBar in transport", 0, 0, 9999999);
    currentBeat  = addNewParameter<IntParameter> ("Current Beat", "currentBeat in transport", 0, 0, 999999);
    beatPerBar = addNewParameter<IntParameter> ("Beat Per Bar", "beat Per Bar", 4, 1, 8);
    playTrigger =  addNewParameter<Trigger> ("Play", "trigger play");
    stopTrigger =  addNewParameter<Trigger> ("Stop", "trigger stop");
    quantizedBarFraction = addNewParameter<IntParameter> ("Global Quantization", "Global quantization in fraction of a bar", 1, 0, 16);
    tapTempo =  addNewParameter<Trigger> ("Tap Tempo", "tap at least 2 times to set the tempo");
    click = addNewParameter<BoolParameter> ("Metronome", "Play the metronome click", false);
    clickVolume = addNewParameter<FloatParameter> ("Metronome Volume", "Click's volume if metronome is active", .5f, 0.f, 1.f);
    setBPMInternal (BPM->doubleValue(), false);

    linkEnabled = addNewParameter<BoolParameter> ("Link", "activate Link", false);
    linkEnabled->enabled = LINK_SUPPORT;

    linkNumPeers = addNewParameter<IntParameter> ("Link Peers", "number of connected Link devices", 0, 0, 32);
    linkNumPeers->isSavable = false;
    linkNumPeers->enabled = LINK_SUPPORT;
    linkNumPeers->setInternalOnlyFlags(true,false);


#if LINK_SUPPORT

    linkPimpl = std::make_unique< LinkImpl> (this);

#endif
    linkLatencyParam = addNewParameter<FloatParameter> ("Link Latency", "Link latency to add for lgml (ms)", 10.f, 0.f, 1000.f);
    linkLatencyParam->setInternalOnlyFlags(true,false);

    clickFader = std::make_unique< FadeInOut> (10000, 10000, true, 1.0 / 3.0);


}
TimeManager::~TimeManager()
{

}


void TimeManager::incrementClock (int block)
{
    audioClock += block;
    jassert (blockSize != 0);

    if (block != blockSize)
    {
#if !LGML_UNIT_TESTS
        jassertfalse;
#endif
        setBlockSize (block);

    }

#if LINK_SUPPORT
    linkPimpl->updateTime();
    linkPimpl->captureTimeLine();
#endif

    updateState();

    if (_isLocked)
    {

        return;
    }

    checkCommitableParams();
    hasJumped = updateAndNotifyTimeJumpedIfNeeded();


    if (!hasJumped  )
    {
        if(timeState.isPlaying){
        timeState.time += blockSize;
        timeState.looseTime=timeState.time;
        }
        else{
            timeState.looseTime+=blockSize;
        }
    }

    timeState.nextTime = timeState.time + blockSize;
    int lastBeat =  int (currentBeat->doubleValue());
    int newBeat = getBeatInt();
    bool isNewBeat =lastBeat != newBeat;
    bool isNewBar = false;
    if (isNewBeat)
    {
        currentBeat->setValueFrom (this,newBeat);

        isNewBar = newBeat % ((int)beatPerBar->value) == 0;
        if(isNewBar)
        {
            currentBar->setValueFrom (this,getBar());

        }
    }

#if LINK_SUPPORT

    // notify link if jump
    if (hasJumped && isPlaying() )
    {
        linkPimpl->notifyJump();

    }


    if (linkEnabled->boolValue() && !timeMasterCandidate && ((isPlaying() &&isNewBar) || hasJumped))
    {
        
        linkPimpl->checkDrift();

    }

#endif


    desiredTimeState = timeState;

    if (hadMasterCandidate && !isSettingTempo->boolValue())
    {
        timeMasterCandidate = nullptr;
    }

    hadMasterCandidate = timeMasterCandidate != nullptr;

    pushCommitableParams();
    


}
void TimeManager::checkCommitableParams()
{
    if (BPM->hasCommitedValue)
    {
        BPM->pushValue();
        setBPMInternal (BPM->doubleValue(), true);
        clickFader->startFadeOut();
#if LINK_SUPPORT
        linkPimpl->commitBPM (BPM->doubleValue());

#endif
    }
}

void TimeManager::pushCommitableParams()
{
    //  BPM->pushValue();
}

bool TimeManager::updateAndNotifyTimeJumpedIfNeeded()
{

    if (timeState.isJumping)
    {
        jassert (blockSize != 0);
        timeState.time = timeState.nextTime;
        timeState.looseTime = timeState.time;
        timeState.nextTime = timeState.time + blockSize;
        timeState.isJumping = false;
        timeManagerListeners.call (&TimeManagerListener::timeJumped, timeState.time);
        desiredTimeState = timeState;

        return true;
    }

    return false;
}

void TimeManager::audioDeviceIOCallback (const float** /*inputChannelData*/,
                                         int /*numInputChannels*/,
                                         float** outputChannelData,
                                         int numOutputChannels,
                                         int numSamples)
{

    if (click->boolValue() && timeState.isPlaying && timeState.time >= 0)
    {
        static sample_clk_t sinCount = 0;

        bool isFirstBeat = (getClosestBeat() % beatPerBar->intValue()) == 0;
        const int sinFreq = sampleRate / (isFirstBeat ? 1320 : 880);
        //    const int sinPeriod = sampleRate / sinFreq;
        //    const double k = 40.0;



        if (desiredTimeState.isJumping)
        {
            clickFader->startFadeOut();
        }

        double cVol = float01ToGain (clickVolume->floatValue());

        for (int i = 0 ; i < numSamples; i++)
        {

            double x = (fmod (getBeatInNextSamples (i), 1.0) ) * beatTimeInSample * 1.0 / sampleRate ;
            double h = jmax (0.0, jmin (1.0, (0.02 - x) / 0.02));

            if ((timeState.time + i) % beatTimeInSample == 0)
            {
                sinCount = 0;
            }

            double carg = sinCount * 1.0 / sinFreq;



            clickFader->incrementFade();
            double cFade = clickFader->getCurrentFade();

            double env = cVol * cFade * jmax (0.0, h); //*exp(1.0-h));
                                                       // jassert(env < 1.0);
            float res = ( env * (sin (2.0 * M_PI * carg ) + 0.1 * sin (2.0 * M_PI * 4.0 * carg )));

            for (int c = 0 ; c < numOutputChannels ; c++ ) {outputChannelData[c][i] = res;}

            sinCount = (sinCount + 1) % (sinFreq);
            //      DBG(clickFader->getCurrentFade());

        }
    }
    else
    {
        for (int i = 0; i < numOutputChannels; ++i)
            zeromem (outputChannelData[i], sizeof (float) * (size_t) numSamples);
    }
//    static int count(0);
//    LOGW("tst : "+String(count++));

#if !LGML_UNIT_TESTS
    incrementClock (numSamples);
#endif

}

bool TimeManager::isPlaying()
{
    return timeState.isPlaying;
}
bool TimeManager::isFirstPlayingFrame()
{
    return desiredTimeState.isPlaying && !timeState.isPlaying;
}
bool TimeManager::isJumping()
{
    return timeState.isJumping;
}

bool TimeManager::askForBeingMasterCandidate (TimeMasterCandidate* n)
{

    if (timeMasterCandidate == nullptr && !isAnyoneBoundToTime() )
    {
        timeMasterCandidate = n;
        isSettingTempo->setValue (true, false, false);
        return true;
    }

    return timeMasterCandidate == n;
}

bool TimeManager::isMasterCandidate (TimeMasterCandidate* n)
{
    return  n == timeMasterCandidate;
}
bool TimeManager::hasMasterCandidate()
{
    return timeMasterCandidate != nullptr;
}
void TimeManager::releaseMasterCandidate (TimeMasterCandidate* n)
{
    jassert (timeMasterCandidate == n);
    isSettingTempo->setValue (false);
}
void TimeManager::releaseIfMasterCandidate (TimeMasterCandidate* n)
{
    if (n == timeMasterCandidate)
    {
        isSettingTempo->setValue (false);
        timeMasterCandidate = nullptr;
    }
}

void TimeManager::play (bool _shouldPlay)
{
    if (!_shouldPlay)
    {
        shouldStop();
        clickFader->startFadeOut();
    }
    else
    {
        //shouldGoToZero();
        shouldPlay();
        clickFader->startFadeIn();
    }
}

void TimeManager::onContainerParameterChanged ( ParameterBase* p)
{
    if (p == playState)
    {
        play (playState->boolValue());
    }

    else if (p == BPMLocked)
    {
        BPM->isEditable = !BPMLocked->boolValue();
    }
    else if( p==currentBar){
        int diff = currentBar->intValue() - (int)currentBar->lastValue;
        currentBeat-> setValue(currentBeat->intValue()+diff*beatPerBar->intValue()); // will recall this (no setValueFrom)
    }
    else if(p==currentBeat){
        int diff = currentBeat->intValue()-getBeatInt() ;
        if(diff!=0){
            goToTime (timeState.time + diff*beatTimeInSample,false);

        }
        else{
            jassertfalse;
        }
    }

#if LINK_SUPPORT
    else if (p == linkEnabled)
    {


        linkPimpl->enable (linkEnabled->boolValue());

    }
    else if (p == linkLatencyParam)
    {


        linkPimpl->linkLatency = std::chrono::microseconds ((long long) (linkLatencyParam->doubleValue() * 1000.0));
    }

#endif
};

void TimeManager::shouldStop (bool now)
{
    desiredTimeState.isPlaying = false;
    goToTime (0, now);

}

void TimeManager::shouldRestart (bool playing)
{
    desiredTimeState.isPlaying = playing;
    goToTime ( 0);
}
void TimeManager::shouldGoToZero()
{
    goToTime (0);
}
void TimeManager::advanceTime (sample_clk_t a, bool now)
{
    if (now)
    {
        goToTime (desiredTimeState.nextTime + a, true);
    }
    else
    {
        if (desiredTimeState.isJumping) {desiredTimeState.nextTime += a;}
        else                          {desiredTimeState.jumpTo (timeState.nextTime + a);}
    }

}

void TimeManager::jump (int amount)
{
    goToTime (timeState.time + amount);


}
void TimeManager::goToTime (sample_clk_t time, bool now)
{

    desiredTimeState.jumpTo (time);

    if (now )
    {

        updateState();
        updateAndNotifyTimeJumpedIfNeeded();
    }

}


void TimeManager::shouldPlay (bool now)
{
    desiredTimeState.isPlaying = true;

    if (now)
    {
        timeState.isPlaying = true;
        updateState();
    }
}

void TimeManager::updateState()
{
    
    String dbg;

    if (timeState.isPlaying != desiredTimeState.isPlaying)
    {
        dbg += "play:" + String (timeState.isPlaying ? "1" : "0") + "/" + (desiredTimeState.isPlaying ? "1" : "0");
        if(playState->boolValue()!=desiredTimeState.isPlaying){
            playState->setValueFrom(this,desiredTimeState.isPlaying);
        }
        else{
        timeManagerListeners.call (&TimeManagerListener::playStop, desiredTimeState.isPlaying);
        }
    }

    if (timeState.time != desiredTimeState.time)
    {
        dbg += " ::: time:" + String (timeState.time) + "/" + String (desiredTimeState.time);
        desiredTimeState.isJumping = true;
        //timeManagerListeners.call (&TimeManagerListener::timeJumped, desiredTimeState.time);
    }

    if (desiredTimeState.isJumping)
    {
        int delta = (int)desiredTimeState.nextTime - (int)timeState.nextTime;

        if (abs (delta) > 32)
        {
            dbg += "time jumping to : " + String (desiredTimeState.nextTime) + " delta=" + String (delta);
        }

        //timeManagerListeners.call (&TimeManagerListener::timeJumped, desiredTimeState.time);
    }

    //  if(dbg!=""){
    //    LOG(dbg);
    //  }





    timeState = desiredTimeState;
}

void TimeManager::onContainerTriggerTriggered (Trigger* t)
{
    if (t == playTrigger)
    {
        if (!playState->boolValue())playState->setValue (true);

        //shouldRestart (true);

        //    desiredTimeState.jumpTo(0);
        //    playState->setValue(false);

    }
    else if (t == stopTrigger)
    {
        playState->setValue (false);
        isSettingTempo->setValue (false, false, false);
    }

    else if (t == tapTempo)
    {
        if (!BPMLocked->boolValue())
        {

            if (!playState->boolValue())
            {
                playState->setValue (true);
                currentBeatPeriod = (sample_clk_t)(beatTimeInSample*1000.0/sampleRate);
            }

            sample_clk_t currentTime = Time().getMillisecondCounter();//timeState.time;
            sample_clk_t delta = currentTime - lastTaped;
            lastTaped = currentTime;

            if ((delta > 100) && (delta < 1000))
            {
                //        const int maxTapInRow = 4;
                tapInRow = (tapInRow + 1) % beatPerBar->intValue();
                const double alpha = .25; // 1. - tapInRow/maxTapInRow *.75 ;
                currentBeatPeriod = alpha * delta + (1 - alpha) * currentBeatPeriod;

                //        int targetBeatInt = tapInRow;

                double targetBeat = getBeat();

                if (targetBeat - (int)targetBeat > 0.5) {targetBeat += 1;}

                int targetBeatInt = floor (targetBeat);

                //        int targetBeatInt = ceil(targetBeat);

                goToTime (targetBeatInt * currentBeatPeriod * sampleRate / 1000.0); //(deltaBeat*beatTimeInSample);

                BPM->setValue (60000.0 / currentBeatPeriod);
                DBG (targetBeatInt << "," << BPM->doubleValue());



            }
            else if (tapInRow > 0) {tapInRow --;}

        }

    }
}
void TimeManager::togglePlay()
{
    if (playState->boolValue())
    {
        stopTrigger->trigger();
    }
    else
        playTrigger->trigger();
}



void TimeManager::setSampleRate (int sr)
{
    sampleRate = sr;
    // actualize beatTime in sample
    beatTimeInSample = (sample_clk_t) (sampleRate * 1.0 / BPM->doubleValue() * 60.0);
}


void TimeManager::setBlockSize (int bS)
{
    jassert (bS != 0);
    blockSize = bS;
    if(bS!=0&& sampleRate!=0){
        if(AudioIODevice* dev = getAudioDeviceManager().getCurrentAudioDevice()){
            // heuristical default value for link Latency
            float heuristicValue  =(dev->getOutputLatencyInSamples() + dev->getCurrentBufferSizeSamples())*1000.0/dev->getCurrentSampleRate();
            linkLatencyParam->defaultValue = heuristicValue;

            if(!linkLatencyParam->isOverriden ){
                linkLatencyParam->resetValue();
            }
        }
    }



}
void TimeManager::setBPMInternal (double /*_BPM*/, bool adaptTimeInSample)
{
    isSettingTempo->setValue (false, false, false);
    sample_clk_t newBeatTime = (sample_clk_t) (sampleRate * 60.0 / BPM->doubleValue() + 0.5);
    double qBPM = sampleRate*60.0/newBeatTime;
    jassert(abs(qBPM - BPM->doubleValue())<0.1);
    BPM->setValueFrom(this,qBPM,false,true);
    if (adaptTimeInSample)
    {
        sample_clk_t targetTime = (sample_clk_t) (timeState.time * (newBeatTime * 1.0 / beatTimeInSample) +0.5);
        jassert (targetTime >= 0);
        beatTimeInSample = newBeatTime;
        goToTime (targetTime, true);
    }
    else{
    beatTimeInSample = newBeatTime;
    }

}
sample_clk_t TimeManager::getTimeInSample()
{
    return timeState.time;
}
sample_clk_t TimeManager::getNextTimeInSample()
{
    if (desiredTimeState.isJumping)return desiredTimeState.nextTime;
    else return  timeState.nextTime;
}

bool  TimeManager::willRestart()
{
    return (timeState.nextTime != 0) && (desiredTimeState.nextTime == 0);
}





TransportTimeInfo TimeManager::findTransportTimeInfoForLength (sample_clk_t time, double _sampleRate)
{
    TransportTimeInfo res;

    if (_sampleRate > 0)
    {
        res.sampleRate = _sampleRate;
    }
    else
    {
        res.sampleRate = sampleRate;
    }

    res.barLength = 1;
    double time_seconds = time * 1.0 / res.sampleRate;
    res.beatTime = time_seconds * 1.0 / beatPerBar->intValue();
    if(res.beatTime == 0){
        jassertfalse;
        res.beatTime = 1;
    }
    // over 150 bpm (0.4s)
    while (res.beatTime < beatTimeGuessRange.getStart()) {res.beatTime *= 2.0; res.barLength /= 2.0;}

    // under 70 bpm (0.85s)
    while (res.beatTime > beatTimeGuessRange.getEnd() ) {res.beatTime /= 2.0; res.barLength *= 2.0;}

    res.beatInSample = (res.beatTime * res.sampleRate);

    res.makeValidForGranularity (samplePerBeatGranularity);

    res.bpm = 60.0 / res.beatTime;
    DBGTIME ("found beat Sample : " << String (res.beatInSample) << " : " << time);

    return res;
}
void TimeManager::setBPMFromTransportTimeInfo (const TransportTimeInfo& info, bool adaptTimeInSample, sample_clk_t atSample)
{

    BPM->setValue (info.bpm, false, false);
    //  sample_clk_t targetTime = getTimeInSample();

    if (adaptTimeInSample)
    {
        sample_clk_t targetTime = timeState.time * (info.beatInSample * 1.0 / beatTimeInSample);
        goToTime (targetTime, true);
    }
    else
    {
        goToTime (atSample, true);
    }

    // force exact beatTimeInSample
    beatTimeInSample = info.beatInSample;
    timeManagerListeners.call (&TimeManagerListener::BPMChanged, BPM->doubleValue());

#if LINK_SUPPORT

    linkPimpl->setBPMNow (info.bpm, std::chrono::microseconds ((long long) (atSample * 1000000.0 / sampleRate)));

#endif
    //jassert((int)(barLength*beatPerBar->intValue())>0);

    // lockBPM for now


    //  BPMLocked->setValue(true);


}

sample_clk_t TimeManager::getNextGlobalQuantifiedTime()
{
    if (willRestart())return 0;

    return getNextQuantifiedTime (quantizedBarFraction->intValue());
}
sample_clk_t TimeManager::getNextQuantifiedTime (int barFraction)
{
    if (willRestart())return 0;

    if (barFraction == -1) {barFraction = quantizedBarFraction->intValue();}

    sample_clk_t nextPosTime = jmax ((sample_clk_t)0, timeState.time);

    if (barFraction == 0) {return nextPosTime;}

    const double samplesPerUnit = (beatTimeInSample * beatPerBar->intValue() * 1.0 / barFraction);
    const sample_clk_t res = (sample_clk_t) ((floor (nextPosTime * 1.0 / samplesPerUnit) + 1) * samplesPerUnit);
    return res;
}

sample_clk_t TimeManager::getTimeForNextBeats (int beats) {return (getBeatInt() + beats) * beatTimeInSample;}

int     TimeManager::getBeatInt()   {return (int)floor (getBeat());}
double  TimeManager::getBeat()      {return (double) (timeState.time * 1.0 / beatTimeInSample);}
double  TimeManager::getLooseBeat()      {return (double) (timeState.looseTime * 1.0 / beatTimeInSample);}
int     TimeManager::getClosestBeat() {return (int)floor (getBeat() + 0.5);}
double  TimeManager::getBeatPercent() {return (double) (timeState.time * 1.0 / beatTimeInSample - getBeatInt());}
double  TimeManager::getBeatInNextSamples (int numSamplesToAdd) {return ((double) (timeState.time + numSamplesToAdd) * 1.0 / beatTimeInSample);}
double TimeManager::getBeatForQuantum (const double q) {return floor (getBeat() / q) * q;}

int TimeManager::getBar() {return (int) (floor (getBeatInt() * 1.0 / beatPerBar->intValue() ));}

void TimeManager::lockTime (bool s)
{
    _isLocked = s;
}
bool TimeManager::isLocked()
{
    return _isLocked;
}
bool TimeManager::getCurrentPosition (CurrentPositionInfo& result)
{
    result.bpm = BPM->doubleValue();
    result.isPlaying = playState->boolValue();
    result.isRecording = isSettingTempo->boolValue();

    result.ppqPosition = (double) (getBeat() ); //??
    result.ppqPositionOfLastBarStart = (double) (getBar() * beatPerBar->intValue()); // ?? 
    result.ppqLoopStart = 0;
    result.ppqLoopEnd = 0;
    result.timeSigNumerator = beatPerBar->intValue();
    result.timeSigDenominator = 4;
    result.timeInSamples = timeState.time;
    result.timeInSeconds = (double) (timeState.time) * sampleRate;
    result.editOriginTime = 0;
    result.frameRate = FrameRateType::fpsUnknown;
    result.isLooping = false;
    return true;
}
bool TimeManager::isAnyoneBoundToTime()
{
    auto allListeners = timeManagerListeners.getListeners();

    for (auto& l : allListeners)
    {
        if (l->isBoundToTime())
        {
            return true;
        }
    }

    return false;
}

void TimeManager::notifyListenerCleared()
{

    if (isAnyoneBoundToTime())
    {
        return;
    }

    shouldRestart (false);
}
