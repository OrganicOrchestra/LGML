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

#include "MIDIClock.h"


MIDIClock::MIDIClock(bool _sendSPP)
:Thread("MIDIClock"),midiFifo(MIDI_SYNC_QUEUE_SIZE)
{
    TimeManager::getInstance()->addTimeManagerListener(this);
    constexpr double framesPerSecond = 30;
    interval = (1000.0/(4.0*framesPerSecond));
    delta = 0.0;

    sendSPP = _sendSPP;

}

MIDIClock::~MIDIClock()
{
    if(auto tm  =TimeManager::getInstanceWithoutCreating()){
        tm->removeTimeManagerListener(this);
    }
    stopThread(2000);
}

bool MIDIClock::setOutput(MIDIListener * _midiOut)
{

    midiOut = _midiOut;
    if(!midiOut)
        return false;

    reset();
    return true;
}

bool MIDIClock::start()
{
    startThread(realtimeAudioPriority);
    return midiOut != nullptr;

}

void MIDIClock::stop()
{
    if(!midiOut)
        return;

    // TODO non blocking stop?
    stopThread(0);
}

void MIDIClock::run()
{

    state.ppqn = getPPQWithDelta(24);

    int st1,st2,bs1,bs2,i;
    while (! threadShouldExit())
    {

        addClockIfNeeded();
        int numR = midiFifo.getNumReady();
        midiFifo.prepareToRead(numR, st1, bs1, st2, bs2);
        for( i = 0;i< bs1 ; i++){
            midiOut->sendMessage(messagesToSend[st1+i]);
        }
        for( i = 0;i< bs2 ; i++){midiOut->sendMessage(messagesToSend[st2+i]);}
        midiFifo.finishedRead(bs1+bs2);

        const int timeToWait = roundToInt(interval );
        wait (timeToWait);
    }
}



double MIDIClock::getPPQWithDelta(int multiplier){
    auto tm = TimeManager::getInstance();
    return (tm->getBeat()+ delta*1.0*tm->BPM->doubleValue()/60000.0)*1.0*multiplier ;
}


void MIDIClock::addClockIfNeeded(){
    if(!TimeManager::getInstance()->isPlaying() || TimeManager::getInstance()->isSettingTempo->boolValue())return;
    int newT = getPPQWithDelta(24);



    JUCE_CONSTEXPR int maxClockMsg = MIDI_SYNC_QUEUE_SIZE - 4;
    int numClocksToAdd  = (newT - state.ppqn);
    if(numClocksToAdd<0){
        DBG(String("waiting clk : ")+ String(numClocksToAdd));
        return;
    }
    if(numClocksToAdd>=0 && !state.isPlaying){
        if(sendSPP)sendCurrentSPP();
        if(!state.isPlaying)sendOneMsg(MidiMessage::midiStart());
        else sendOneMsg(MidiMessage::midiContinue());
        state.isPlaying=true;
    }
    if(numClocksToAdd > maxClockMsg){
        jassertfalse;
        numClocksToAdd = maxClockMsg;

    }

    sendClocks(numClocksToAdd);

}

void MIDIClock::sendCurrentSPP(){
    if(sendSPP){
        jassert(!state.isPlaying);
        //        int64 MIDIBeat = (int64)getPPQWithDelta(4);
        auto tm = TimeManager::getInstance();
        int MIDIBeat = (tm->getBeat() )*4.0;
        if(MIDIBeat<0){
//            jassertfalse;
        }
        // SPP Msg
       sendOneMsg(MidiMessage::songPositionPointer(MIDIBeat));


    }
    
}


void MIDIClock::reset()
{
    playStop(false);
}



void MIDIClock::BPMChanged (double /*BPM*/) {}

void MIDIClock::timeJumped (sample_clk_t time) {
    if(!state.isPlaying){
        sendCurrentSPP();
    }

}

void MIDIClock::playStop (bool playStop) {

    if(playStop){

        state.ppqn = getPPQWithDelta(24);
        if(state.ppqn<0){sendClocks(-state.ppqn);}




    }
    else{

        sendOneMsg(MidiMessage::midiStop());
        state.isPlaying=false;

    }

}


void MIDIClock::sendClocks(const int numClocksToAdd){
    int st1,st2,bs1,bs2;
    midiFifo.prepareToWrite(numClocksToAdd, st1, bs1, st2,bs2);
    for(int i = st1 ; i < st1+bs1 ; i++){
        messagesToSend[i] = MidiMessage::midiClock();
    }
    for(int i = st2 ; i < st2+bs2 ; i++){messagesToSend[i] = MidiMessage::midiClock();}
    midiFifo.finishedWrite(bs1+bs2);
    state.ppqn+=numClocksToAdd;


}


void MIDIClock::sendOneMsg(const MidiMessage & msg){
    int st1,st2,bs1,bs2;
    midiFifo.prepareToWrite(1,st1,bs1,st2,bs2);
    if(bs1>0){
        messagesToSend[st1] = msg;
    }
    midiFifo.finishedWrite(1);
}

