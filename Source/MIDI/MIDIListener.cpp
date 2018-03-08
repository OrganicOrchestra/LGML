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

#include "MIDIListener.h"
#include "../Utils/DebugHelpers.h"


String getClosestOutName(const String  &midiPortName);
int commonNamePartWithInName (String& s,const String & midiPortName);

MIDIListener::MIDIListener():hasValidPort(true)
{
    midiPortName = String::empty;
    outPortName = String::empty;
    MIDIManager::getInstance()->addMIDIListener (this);

}

MIDIListener::~MIDIListener()
{
    if(auto mm  = MIDIManager::getInstanceWithoutCreating()){
    mm->removeMidiInputCallback (midiPortName, this);
    mm->removeMIDIListener (this);
    }
}

void MIDIListener::setCurrentDevice (const String& deviceName)
{
    //if (deviceName == midiPortName) return;
    if(!MessageManager::getInstance()->isThisTheMessageThread()){
        MessageManager::getInstance()->callAsync(
                                                 [this,deviceName](){setCurrentDevice(deviceName);});
        return;
    }
    auto mm = MIDIManager::getInstance();

    if (deviceName.isNotEmpty())
    {
        mm->updateLists();
        if (mm->inputDevices.indexOf (deviceName) == -1)
        {
            ghostPortName = deviceName;
            setCurrentDevice (String::empty);
            return;
        }
    }

    if (midiPortName.isNotEmpty())
    {
        mm->disableInputDevice (midiPortName);
        mm->disableOutputDevice (outPortName);
        mm->removeMidiInputCallback (midiPortName, this);
        hasValidPort = false;
    }



    midiPortName = deviceName;


    if (midiPortName.isNotEmpty())
    {
        mm->enableInputDevice (midiPortName);
        outPortName = getClosestOutName(midiPortName);
        midiOutDevice = mm->enableOutputDevice (outPortName);
        mm->addMidiInputCallback (midiPortName, this);
        hasValidPort = true;
    }

    mm->checkMIDIListenerStates();

}

void MIDIListener::sendNoteOn (int channel, int pitch, int velocity)
{
    if (midiOutDevice == nullptr)
    {
        LOGW(juce::translate("MIDI Out is null"));
        return;
    }

    //LOG("Send Note On");

    MidiMessage msg = MidiMessage::noteOn (channel, pitch, (uint8)velocity);
    midiOutDevice->sendMessageNow (msg);
    midiMessageSent();
}

void MIDIListener::sendNoteOff (int channel, int pitch, int velocity)
{
    if (midiOutDevice == nullptr)
    {
        LOGW(juce::translate("MIDI Out is null"));
        return;
    }

    //LOG("Send Note Off");


    MidiMessage msg = MidiMessage::noteOff (channel, pitch, (uint8)velocity);
    midiOutDevice->sendMessageNow (msg);
    midiMessageSent();
}

void MIDIListener::sendMessage(const MidiMessage & msg){
    if (midiOutDevice == nullptr)
    {
        LOGW(juce::translate("MIDI Out is null"));
        return;
    }

    midiOutDevice->sendMessageNow (msg);
    midiMessageSent();
}

void MIDIListener::sendCC (int channel, int number, int value)
{
    if (midiOutDevice == nullptr)
    {
        LOGW(juce::translate("MIDI Out is null"));
        return;
    }

    //LOG("Send CC");
    if(channel==0){
        channel = 1;
    }

    MidiMessage msg = MidiMessage::controllerEvent (channel, number, value);
    midiOutDevice->sendMessageNow (msg);
    midiMessageSent();
}

void MIDIListener::sendSysEx (uint8* data, int dataCount)
{
    if (midiOutDevice == nullptr)
    {
        DBG ("MIDI Out is null");
        return;
    }

    //LOG("Send SysEx");

    MidiMessage msg = MidiMessage::createSysExMessage (data, dataCount);
    midiOutDevice->sendMessageNow (msg);
    midiMessageSent();
}

void MIDIListener::midiInputAdded (String& s)
{
    DBG ("MIDIListener :: inputAdded " << s << ",portName = " << midiPortName << ", ghost = " << ghostPortName);

    if (s == midiPortName)
    {
        setCurrentDevice (midiPortName);
        ghostPortName = String::empty;
    }
    else if (s == ghostPortName)
    {
        setCurrentDevice (ghostPortName);
        ghostPortName = String::empty;
    }
}

void MIDIListener::midiInputRemoved (String& s)
{
    if (s == midiPortName)
    {
        ghostPortName = s;
        setCurrentDevice (String::empty);
    }
}





void MIDIListener::midiOutputAdded (String& s)
{

    if (commonNamePartWithInName (s,midiPortName) > 0)
    {
        setCurrentDevice (midiPortName);
    }
}

void MIDIListener::midiOutputRemoved (String& /*s*/)
{
    //
}


/////////////////////
// Utility
//////////////////

int commonNamePartWithInName (String& s,const String & midiPortName)
{
    StringArray arrOut ;
    arrOut.addTokens (s, true);
    StringArray arrIn ;
    arrIn.addTokens (midiPortName, true);
    int max = jmin (arrOut.size(), arrIn.size());
    int res = 0;

    for (int i = 0 ; i < max ; i++)
    {
        if (arrIn[i] == arrOut[i]) {res++;}
        
    }

    return res;
}

String getClosestOutName(const String  &midiPortName)
{

    int best = 0;
    String bestName = midiPortName;
    MIDIManager::getInstance()->updateLists();
    for (auto& outName : MIDIManager::getInstance()->outputDevices)
    {

        if (outName == midiPortName) {return midiPortName;}

        int n = commonNamePartWithInName (outName,midiPortName);

        if (n > best)
        {
            bestName = outName;
            best = n;
        }
    }

    return bestName;


}

