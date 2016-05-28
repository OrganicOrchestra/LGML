/*
  ==============================================================================

    MIDIListener.cpp
    Created: 12 May 2016 9:42:56am
    Author:  Martin Hermant

  ==============================================================================
*/

#include "MIDIListener.h"

#include "MIDIManager.h"
#include "DebugHelpers.h"
#include "MainComponent.h"

MIDIListener::MIDIListener()
{
}

void MIDIListener::setCurrentDevice(const String & deviceName)
{
    if (deviceName == midiPortName) return;

    if (!midiPortName.isEmpty())
    {
        MIDIManager::getInstance()->disableInputDevice(midiPortName);
        getAudioDeviceManager().removeMidiInputCallback(midiPortName, this);
    }

    midiPortName =  deviceName;


    if (!midiPortName.isEmpty())
    {
        MIDIManager::getInstance()->enableInputDevice(midiPortName);
        getAudioDeviceManager().addMidiInputCallback(midiPortName, this);
    }

    MIDIListenerListeners.call(&MIDIListener::Listener::currentDeviceChanged, this);
}
