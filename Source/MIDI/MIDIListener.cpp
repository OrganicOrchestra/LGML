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

#include "MIDIListener.h"
#include "../Utils/DebugHelpers.h"



int commonNamePartWithInName (String& s,const String & midiPortName);

MIDIListener::MIDIListener():hasValidInPort(true),hasValidOutPort(true)
{
    inPortName = "";
    outPortName = "";
    MIDIManager::getInstance()->addMIDIListener (this);

}

MIDIListener::~MIDIListener()
{
    if(auto mm  = MIDIManager::getInstanceWithoutCreating()){
    mm->removeMidiInputCallback (inPortName, this);
    mm->removeMIDIListener (this);
    }
}

void MIDIListener::setCurrentDevice (const String& deviceName,bool output)
{
    //if (deviceName == inPortName) return;
    if(!MessageManager::getInstance()->isThisTheMessageThread()){
        MessageManager::getInstance()->callAsync(
                                                 [this,deviceName,output](){setCurrentDevice(deviceName,output);});
        return;
    }
    auto mm = MIDIManager::getInstance();

    if (deviceName.isNotEmpty())
    {
        mm->updateLists();
        auto deviceList = output?mm->outputDevices:mm->inputDevices;
        if (deviceList.indexOf (deviceName) == -1)
        {
            if(output){
                ghostOutPortName = deviceName;
            }else{
                ghostInPortName = deviceName;
            }
            setCurrentDevice ("",output);
            return;
        }
    }
    if(outPortName.isNotEmpty() && output){
        mm->disableOutputDevice (outPortName);
        hasValidOutPort = false;
    }
    if (inPortName.isNotEmpty() && !output)
    {
        mm->disableInputDevice (inPortName);
        mm->removeMidiInputCallback (inPortName, this);
        hasValidInPort = false;
    }

    if(output){
        outPortName = deviceName;
        if (outPortName.isNotEmpty())
        {
            midiOutDevice = mm->enableOutputDevice (outPortName);
            hasValidOutPort = true;
        }
    }
    else{
        inPortName = deviceName;
        if (inPortName.isNotEmpty())
        {
            mm->enableInputDevice (inPortName);
            mm->addMidiInputCallback (inPortName, this);
            hasValidInPort = true;
        }
    }



    mm->checkMIDIListenerStates();
    
}

bool MIDIListener::sendNoteOn (int channel, int pitch, int velocity)
{
    if(channel==0){channel = 1;jassertfalse;}
    return sendMessage(MidiMessage::noteOn (channel, pitch, (uint8)velocity));
}

bool MIDIListener::sendNoteOff (int channel, int pitch, int velocity)
{
    if(channel==0){channel = 1;jassertfalse;}
    return sendMessage(MidiMessage::noteOff (channel, pitch, (uint8)velocity));

}

bool  MIDIListener::sendMessage(const MidiMessage & msg){
    if (midiOutDevice == nullptr)
    {
        return false ;
    }

    midiOutDevice->sendMessageNow (msg);
    midiMessageSent(msg);
    return true;
}

bool MIDIListener::sendCC (int channel, int number, int value)
{
    //LOG("Send CC");
    if(channel==0){channel = 1;jassertfalse;}
    return sendMessage(MidiMessage::controllerEvent (channel, number, value));

}

bool  MIDIListener::sendSysEx (uint8* data, int dataCount)
{
    return sendMessage(MidiMessage::createSysExMessage (data, dataCount));
    
}

void MIDIListener::midiInputAdded (String& s)
{
    DBG ("MIDIListener :: inputAdded " << s << ",portName = " << inPortName << ", ghost = " << ghostInPortName);

    if (s == inPortName)
    {
        setCurrentDevice (inPortName,false);
        ghostInPortName = "";
    }
    else if (s == ghostInPortName)
    {
        setCurrentDevice (ghostInPortName,false);
        ghostInPortName = "";
    }
}

void MIDIListener::midiInputRemoved (String& s)
{
    if (s == inPortName)
    {
        ghostInPortName = s;
        setCurrentDevice ("",false);
    }
}





void MIDIListener::midiOutputAdded (String& s)
{
    if(s==outPortName){
        setCurrentDevice(outPortName, true);
        ghostOutPortName = "";
    }
    else if(s==ghostOutPortName){
        setCurrentDevice(ghostOutPortName, true);
        ghostOutPortName = "";
    }

//    else if (commonNamePartWithInName (s,inPortName) > 0)
//    {
//        setCurrentDevice (inPortName,true);
//    }
}

void MIDIListener::midiOutputRemoved (String& s)
{
    if (s == outPortName)
    {
        ghostOutPortName = s;
        setCurrentDevice ("",true);
    }
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

String MIDIListener::getClosestOutName(const String & midiPortName )
{
    
    int best = 0;
    String bestName = midiPortName;
    MIDIManager::getInstance()->updateLists();
    for (auto& outName : MIDIManager::getInstance()->outputDevices)
    {

        if (outName == midiPortName) {return midiPortName;}
        if(outName.startsWith("from ") && midiPortName.startsWith("to ") && midiPortName.substring(3)==outName.substring(5)){return outName;}
        int n = commonNamePartWithInName (outName,midiPortName);

        if (n > best)
        {
            bestName = outName;
            best = n;
        }
    }

    return bestName;


}

