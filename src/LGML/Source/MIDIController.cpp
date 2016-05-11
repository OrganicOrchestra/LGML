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
#include "MIDIManager.h"

AudioDeviceManager & getAudioDeviceManager();

MIDIController::MIDIController() :
	Controller("MIDI"),
	midiPortName("")
{
}

void MIDIController::setCurrentDevice(const String & deviceName)
{
	if (deviceName == midiPortName) return;

	if (!midiPortName.isEmpty())
	{
		MIDIManager::getInstance()->disableInputDevice(midiPortName);
		getAudioDeviceManager().removeMidiInputCallback(midiPortName, this);
	}

	midiPortName = deviceName;

	if (!midiPortName.isEmpty())
	{
		DBG("Enable here ");
		LGMLLogger::getInstance()->logMessage("Open device " + midiPortName);
		MIDIManager::getInstance()->enableInputDevice(midiPortName);
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
	DBG("Callback");
	LGMLLogger::getInstance()->logMessage("Incoming midi message : " + String(source->getName()) + " / " + String(message.getControllerValue()));
    
}