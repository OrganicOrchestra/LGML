/*
  ==============================================================================

    MIDIController.cpp
    Created: 2 Mar 2016 8:51:20pm
    Author:  bkupe

  ==============================================================================
*/

#include "MIDIController.h"
#include "MIDIControllerUI.h"
#include "LGMLLogger.h"

AudioDeviceManager & getAudioDeviceManager();

MIDIController::MIDIController() :
	Controller("MIDI")
{
}

void MIDIController::setCurrentDevice(const String & deviceName)
{
	if (deviceName == midiPortName) return;

	if (!midiPortName.isEmpty())
	{
		getAudioDeviceManager().removeMidiInputCallback(midiPortName, this);
	}

	midiPortName = deviceName;

	if (!midiPortName.isEmpty())
	{
		getAudioDeviceManager().addMidiInputCallback(midiPortName, this);
	}

	midiControllerListeners.call(&MIDIControllerListener::currentDeviceChanged, this);
}

ControllerUI * MIDIController::createUI()
{
	return new MIDIControllerUI(this);
}

void MIDIController::handleIncomingMidiMessage (MidiInput* source,
                                                const MidiMessage& message) 
{
	LGMLLogger::getInstance()->logMessage("Incoming midi message : " + String(source->getName()) + " / " + String(message.getControllerValue()));
    
}