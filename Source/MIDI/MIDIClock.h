/*
  ==============================================================================

    MIDIClock.h
    Created: 1 Mar 2018 6:25:11pm
    Author:  Martin Hermant

  ==============================================================================
*/

#pragma once
#include "MIDIListener.h"
#include "../Time/TimeManager.h"

constexpr int MIDI_SYNC_QUEUE_SIZE = 100;
class MIDIClock: public Thread, private TimeManager::TimeManagerListener
{
public:
    MIDIClock();
    ~MIDIClock();

    bool start();
    void stop();
    bool setOutput(MIDIListener * _midiOut);
    void reset();
    
    void askSyncOnNextBeat();
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
    void sendSPPIfNeeded();
    MidiMessage  messagesToSend[MIDI_SYNC_QUEUE_SIZE];
    AbstractFifo midiFifo;
    int lastppqn;
    bool syncNextBeat;
    double nextBeat;
    void syncPendingBeat();
    void midiContinue();
    double getBeatWithDelta(int multiplier);

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (MIDIClock);
    
};

