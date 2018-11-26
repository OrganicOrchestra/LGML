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
    linkTimeLine (ableton::link::Timeline(), true),
    linkLatency (00),
    linkTime(-1)
    {
        linkSession.setNumPeersCallback (&LinkImpl::linkNumPeersCallBack);
        linkSession.setTempoCallback (&LinkImpl::linkTempoCallBack);
    };
    ableton::Link linkSession;

    ableton::Link::Timeline  linkTimeLine;
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

        linkTimeLine = linkSession.captureAudioTimeline();
        const double quantum = owner->beatPerBar->intValue() / owner->quantizedBarFraction->doubleValue();
        jassert (quantum > 0);

        const double linkBeat = linkTimeLine.beatAtTime (linkTime, quantum);
        //    auto phaseAtTime = linkTimeLine.phaseAtTime(linkTime, tstQ);
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
        linkTimeLine.requestBeatAtTime (owner->getBeat(),
                                        //                      std::chrono::system_clock::now().time_since_epoch(),
                                        linkTime,
                                        owner->beatPerBar->intValue() * 1.0 / owner->quantizedBarFraction->intValue());
        linkSession.commitAudioTimeline (linkTimeLine);

    }
    void captureTimeLine()
    {
        linkTimeLine = linkSession.captureAudioTimeline();
    }

    void commitBPM (double BPM)
    {
        linkTimeLine = linkSession.captureAudioTimeline();
        linkTimeLine.setTempo (BPM, linkTime);
        linkSession.commitAudioTimeline (linkTimeLine);

    }


    void setBPM (double bpm, std::chrono::microseconds delta)
    {
        linkTimeLine = ableton::Link::Timeline (ableton::link::Timeline(), true);

        linkTimeLine.setTempo (bpm, linkTime);
        linkTimeLine.forceBeatAtTime (0, linkTime + delta, 0);

        linkSession.commitAudioTimeline (linkTimeLine);
    }
    void enable (bool b)
    {
        linkSession.enable (b);

        if (b)
        {

            auto lTl = linkSession.captureAppTimeline();
            lTl.requestBeatAtTime (owner->getBeat(),
                                   //                      std::chrono::system_clock::now().time_since_epoch(),
                                   linkTime,
                                   owner->beatPerBar->intValue() * 1.0 / owner->quantizedBarFraction->intValue());
            linkSession.commitAppTimeline (lTl);
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
