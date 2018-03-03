/*
 ==============================================================================

 MIDIClock.cpp
 Created: 1 Mar 2018 6:25:11pm
 Author:  Martin Hermant

 ==============================================================================
 */

#include "MIDIClock.h"



MIDIClock::MIDIClock()
:Thread("MIDIClock"),midiFifo(MIDI_SYNC_QUEUE_SIZE)
{
    TimeManager::getInstance()->addTimeManagerListener(this);
    constexpr double framesPerSecond = 30;
    interval = (1000.0/(4.0*framesPerSecond));
    delta = 0.0;

    sendSPP = true;
    syncNextBeat = false;
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
    startThread(6);

    if(!midiOut)
        return false;

    return true;
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
    {
        auto tm = TimeManager::getInstance();
        lastppqn = getBeatWithDelta(24);
    }
    int st1,st2,bs1,bs2,i;
    while (! threadShouldExit())
    {

        addClockIfNeeded();
        syncPendingBeat();
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

void MIDIClock::syncPendingBeat(){
    if(syncNextBeat){
        auto tm = TimeManager::getInstance();
        float newT = tm->getBeat();//getBeatWithDelta(1);
        if(newT>nextBeat){
            syncNextBeat = false;
            float drift =(newT-nextBeat)*60000.0/TimeManager::getInstance()->BPM->doubleValue();
            DBG("MIDI drift :  "<< drift);
            // jassert(fabs(drift)<10);
            //int numR = midiFifo.getNumReady();
            //midiFifo.finishedRead(numR);
            playStop(false);
            sendSPPIfNeeded();
            midiContinue();

        }

    }

}

double MIDIClock::getBeatWithDelta(int multiplier){
    auto tm = TimeManager::getInstance();
    return (tm->getBeat()+ delta*1.0*tm->BPM->doubleValue()/60000.0)*1.0*multiplier ;
}

void MIDIClock::askSyncOnNextBeat(){
    int inc = 1;
    auto tm = TimeManager::getInstance();
    if(tm->isPlaying()){
        if( !syncNextBeat){
            nextBeat  = getBeatWithDelta(1)+inc;
            while(nextBeat<=0){
                nextBeat+=inc;
            }
            syncNextBeat = true;
        }
        //        else{
        //            nextBeat = jmax(nextBeat,getBeatWithDelta(1)+inc);
        //        }

    }
    else{
        syncNextBeat = false;
        nextBeat = 0;
    }
    //DBG("nextBeat : " << nextBeat);
}
void MIDIClock::addClockIfNeeded(){
    if(!TimeManager::getInstance()->isPlaying())return;
    int newT = getBeatWithDelta(24);

    JUCE_CONSTEXPR int maxClockMsg = MIDI_SYNC_QUEUE_SIZE - 4;
    int numClocksToAdd  = (newT - lastppqn);
    if(numClocksToAdd<0){
        if(!syncNextBeat){
            int dbg;dbg++;
        }
        return;
    }
    if(numClocksToAdd > maxClockMsg){
        jassertfalse;
        numClocksToAdd = maxClockMsg;

    }
    int st1,st2,bs1,bs2;
    midiFifo.prepareToWrite(numClocksToAdd, st1, bs1, st2,bs2);
    for(int i = st1 ; i < st1+bs1 ; i++){
        messagesToSend[i] = MidiMessage::midiClock();
    }
    for(int i = st2 ; i < st2+bs2 ; i++){messagesToSend[i] = MidiMessage::midiClock();}
    midiFifo.finishedWrite(bs1+bs2);


    lastppqn = newT;
}

void MIDIClock::sendSPPIfNeeded(){
    if(sendSPP){


        //        int64 MIDIBeat = (int64)getBeatWithDelta(4);
        auto tm = TimeManager::getInstance();
        int64 MIDIBeat = (int64)((tm->getBeat()  - delta*tm->BPM->doubleValue()/60000.0 )*4.0);
        if(MIDIBeat<0){
            jassertfalse;
        }
        // SPP Msg
        MidiMessage msg(0xF2,(int)(MIDIBeat & 0x7F),(int)((MIDIBeat>>7) & 0x7F));
        int st1,st2,bs1,bs2;
        midiFifo.prepareToWrite(1, st1, bs1, st2,bs2);
        if(bs1>0)
            messagesToSend[st1] = msg;
        else
            jassertfalse;
        midiFifo.finishedWrite(bs1);
    }
}


void MIDIClock::reset()
{

    askSyncOnNextBeat();
}



void MIDIClock::BPMChanged (double /*BPM*/) {}

void MIDIClock::timeJumped (sample_clk_t time) {

    askSyncOnNextBeat();


}

void MIDIClock::playStop (bool playStop) {
    int st1,bs1,st2,bs2;
    midiFifo.prepareToWrite(1, st1, bs1, st2, bs2);
    if(bs1 >0){
        if(playStop){
            auto tm = TimeManager::getInstance();

            //lastppqn =tm->getBeat()*24;
            lastppqn = getBeatWithDelta(24);

            if(tm->getBeat()==0 && delta==0){
                messagesToSend[st1] = MidiMessage::midiStart();
            }
            else{
                askSyncOnNextBeat();
                messagesToSend[st1] = MidiMessage::midiContinue();
            }
        }
        else{

            messagesToSend[st1] = MidiMessage::midiStop();
            syncNextBeat = false;
            nextBeat=0;

        }


    }
    else{
        jassertfalse;
    }
    midiFifo.finishedWrite(1);
}

void MIDIClock::midiContinue () {
    int st1,bs1,st2,bs2;
    midiFifo.prepareToWrite(1, st1, bs1, st2, bs2);
    if(bs1 >0){messagesToSend[st1] = MidiMessage::midiContinue();}
    midiFifo.finishedWrite(1);
}
