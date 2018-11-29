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

#pragma once

#if LINK_SUPPORT
#include "ableton/Link.hpp"
#include "ableton/link/HostTimeFilter.hpp"

class LinkImpl
{
public:
    LinkImpl (TimeManager* o): owner (o), linkSession (120.0),
    linkSessionState (ableton::link::ApiState(), true),
    linkLatency (00),
    linkTime(-1)
    {
        linkSession.setNumPeersCallback (&LinkImpl::linkNumPeersCallBack);
        linkSession.setTempoCallback (&LinkImpl::linkTempoCallBack);
    };
    ableton::Link linkSession;

    ableton::Link::SessionState  linkSessionState;
    std::chrono::microseconds  linkTime;
    //    ableton::link::HostTimeFilter<ableton::link::platform::Clock> linkFilter;
    std::chrono::microseconds linkLatency;

    void updateTime(){
        auto nlinkTime = std::chrono::microseconds((long long)(Time::getMillisecondCounterHiRes()*1000.0))+linkLatency;
        if(nlinkTime>linkTime){ // link time has to be monotonic, so take care of linkLatency changes
            linkTime = nlinkTime;
        }
        //        linkTime = linkFilter.sampleTimeToHostTime (owner->audioClock) + linkLatency;
    }
    void checkDrift()
    {

        linkSessionState = linkSession.captureAudioSessionState();
        const double quantum = owner->beatPerBar->intValue() / owner->quantizedBarFraction->doubleValue();
        jassert (quantum > 0);

        const double linkBeat = linkSessionState.beatAtTime (linkTime, quantum);
        //    auto phaseAtTime = linkSessionState.phaseAtTime(linkTime, tstQ);
        //    auto localPhase = fmod(localBeat,tstQ);
        const double localBeat = owner->getBeat();
        const float driftMs = (linkBeat - localBeat) * owner->beatTimeInSample * 1000.0f / owner->sampleRate;

        if (//(owner->isPlaying() )&&
            fabs (driftMs) > 5
            )
        {
            SLOG ("! link drift : " + String (driftMs) + "ms");
            owner->goToTime (linkBeat  * owner->beatTimeInSample, true);
        }
    }

    void notifyJump()
    {
        linkSessionState.requestBeatAtTime (owner->getBeat(),
                                        linkTime,
                                        owner->beatPerBar->intValue() * 1.0 / owner->quantizedBarFraction->intValue());
        linkSession.commitAudioSessionState (linkSessionState);

    }
    void captureTimeLine()
    {
        linkSessionState = linkSession.captureAudioSessionState();
    }

    void commitBPM (double BPM)
    {
        linkSessionState = linkSession.captureAudioSessionState();
        linkSessionState.setTempo (BPM, linkTime);
        linkSession.commitAudioSessionState (linkSessionState);

    }


    void setBPMNow (double bpm, std::chrono::microseconds delta)
    {
        linkSessionState = ableton::Link::SessionState (ableton::link::ApiState(), true); // we force other peers to jump

        linkSessionState.setTempo (bpm, linkTime);
        linkSessionState.forceBeatAtTime (0, linkTime + delta, 0);

        linkSession.commitAudioSessionState (linkSessionState);
    }
    void enable (bool b)
    {
        linkSession.enable (b);

        if (b)
        {

            auto lTl = linkSession.captureAppSessionState();
            lTl.requestBeatAtTime (owner->getBeat(),
                                   //                      std::chrono::system_clock::now().time_since_epoch(),
                                   linkTime,
                                   owner->beatPerBar->intValue() * 1.0 / owner->quantizedBarFraction->intValue());
            linkSession.commitAppSessionState (lTl);
        }
    }

    static void linkTempoCallBack (const double tempo)
    {
        if (TimeManager* tm = TimeManager::getInstanceWithoutCreating())
        {
            tm->BPM->setValue (tempo);
        }

    }
    static void linkNumPeersCallBack (const size_t numPeers)
    {
        if (TimeManager* tm = TimeManager::getInstanceWithoutCreating())
        {
            tm->linkNumPeers->setValue ((int)numPeers);
        }
        
    }
    
    TimeManager* owner;
};
#else
class LinkImpl
{
};
#endif
