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
#include "MIDIListener.h"
#include "../Time/TimeManager.h"

constexpr int MIDI_SYNC_QUEUE_SIZE = 100;
class MIDIClock: public Thread, private TimeManager::TimeManagerListener
{
public:
    MIDIClock(bool sendSPP);
    ~MIDIClock();

    bool start();
    void stop();
    bool setOutput(MIDIListener * _midiOut);
    void reset();

    bool sendSPP;
    float delta;
    

private:
    void run()override;

    MidiMessage nextMidiMsg;
    MIDIListener* midiOut;

    float interval;
    


    // TimeManager Listener

     void BPMChanged (double /*BPM*/) override ;
     void timeJumped (sample_clk_t /*time*/) override ;
     void playStop (bool /*playStop*/) override ;
    // info for stopping manager if needed;
     bool isBoundToTime() override {return false;};
    void addClockIfNeeded();
    void sendCurrentSPP();



    
    double getPPQWithDelta(int multiplier);

    void sendOneMsg(const MidiMessage & msg);
    void sendClocks(const int num);
    MidiMessage  messagesToSend[MIDI_SYNC_QUEUE_SIZE];
    AbstractFifo midiFifo;

    private :
    struct MIDIClockState{
        MIDIClockState():isPlaying(false),ppqn(0){};
        bool isPlaying;
        int ppqn;

    };
    MIDIClockState state;
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (MIDIClock);
    
};

