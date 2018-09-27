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

#ifndef MIDIMANAGER_H_INCLUDED
#define MIDIMANAGER_H_INCLUDED
#pragma once


#include "../JuceHeaderAudio.h"//keep

class MIDIListener;
class ComputerKeyboardMIDIDevice;
extern AudioDeviceManager& getAudioDeviceManager();


class MIDIManager : private Timer
{
public:
    struct DeviceUsageCount
    {
    public:
        explicit DeviceUsageCount (String name) : deviceName (std::move(name)), usageCount (0) {}
        String deviceName;
        int usageCount;
    };

    juce_DeclareSingleton (MIDIManager, true)

    MIDIManager();
    ~MIDIManager();

    void init();
    void updateLists();
    
    StringArray inputDevices;
    StringArray outputDevices;

    void enableInputDevice (const String& deviceName);
    MidiOutput* enableOutputDevice (const String& deviceName);
    void disableInputDevice (const String& deviceName);
    void disableOutputDevice (const String& deviceName);

    void addMidiInputCallback(const String & deviceName,MidiInputCallback * cb);
    void removeMidiInputCallback(const String & deviceName,MidiInputCallback * cb);

    class  MIDIManagerListener
    {
    public:

        virtual ~MIDIManagerListener() {}
        virtual void midiInputAdded (String&) {}
        virtual void midiInputRemoved (String&) {}
        virtual void midiInputsChanged() {}

        virtual void midiOutputAdded (String&) {}
        virtual void midiOutputRemoved (String&) {}
        virtual void midiOutputsChanged() {}

    };

    ListenerList<MIDIManagerListener> listeners;
    void addMIDIManagerListener (MIDIManagerListener* newListener) { listeners.add (newListener); }
    void removeMIDIManagerListener (MIDIManagerListener* listener) { listeners.remove (listener); }


    void addMIDIListener(MIDIListener * l);
    void removeMIDIListener(MIDIListener * l);
    Array<MIDIListener* > MIDIListeners;
    void checkMIDIListenerStates();
private:
    OwnedArray<DeviceUsageCount> inputCounts;
    OwnedArray<DeviceUsageCount> outputCounts;

    void updateDeviceList (bool updateInput);




    DeviceUsageCount* getDUCForInputDeviceName (const String& deviceName);
    DeviceUsageCount* getDUCForOutputDeviceName (const String& deviceName);

    void timerCallback() override;


    ComputerKeyboardMIDIDevice * computerKeyboardDevice;
    


};


#endif  // MIDIMANAGER_H_INCLUDED
