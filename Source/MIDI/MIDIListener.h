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


    String inPortName;
    String ghostInPortName,ghostOutPortName;
    String outPortName;
    bool hasValidInPort;
    bool hasValidOutPort;

    void setCurrentDevice (const String& deviceName,bool output);

    //Output
    std::unique_ptr<MidiOutput> midiOutDevice;
    bool sendNoteOn (int channel, int pitch, int velocity);
    bool sendNoteOff (int channel, int pitch, int velocity);
    bool sendCC (int channel, int number, int value);
    bool sendSysEx (uint8* data, int dataCount);
    bool sendMessage(const MidiMessage & msg);
    virtual void midiMessageSent(const MidiMessage & ){}
    virtual void midiInputAdded (String& s) override;
    virtual void midiInputRemoved (String& s) override;
    //    virtual void midiInputsChanged() {}

    virtual void midiOutputAdded (String& s) override;
    virtual void midiOutputRemoved (String& s) override;
    //    virtual void midiOutputsChanged() {}

    
    String getClosestOutName(const String &);

    
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (MIDIListener)
    private:
    WeakReference<MIDIListener>::Master masterReference;
    friend class WeakReference<MIDIListener>;
};



#endif  // MIDILISTENER_H_INCLUDED
