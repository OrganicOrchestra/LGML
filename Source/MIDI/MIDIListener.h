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

#ifndef MIDILISTENER_H_INCLUDED
#define MIDILISTENER_H_INCLUDED
#pragma once
#include "MIDIManager.h"

// abstract base class that provide midi callback handle and keeps MIDIManager in sync

class MIDIListener :
public MidiInputCallback,
public MIDIManager::MIDIManagerListener
{
public :
    MIDIListener();
    virtual ~MIDIListener();


    String midiPortName;
    String ghostPortName;
    String outPortName;
    bool hasValidPort;

    virtual void setCurrentDevice (const String& deviceName);

    //Output
    ScopedPointer<MidiOutput> midiOutDevice;
    void sendNoteOn (int channel, int pitch, int velocity);
    void sendNoteOff (int channel, int pitch, int velocity);
    void sendCC (int channel, int number, int value);
    void sendSysEx (uint8* data, int dataCount);
    void sendMessage(const MidiMessage & msg);
    virtual void midiMessageSent(){};
    virtual void midiInputAdded (String& s) override;
    virtual void midiInputRemoved (String& s) override;
    //    virtual void midiInputsChanged() {}

    virtual void midiOutputAdded (String& s) override;
    virtual void midiOutputRemoved (String& s) override;
    //    virtual void midiOutputsChanged() {}

    


    
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (MIDIListener)
};



#endif  // MIDILISTENER_H_INCLUDED
