/*
  ==============================================================================

    MIDIManager.cpp
    Created: 11 May 2016 7:17:05pm
    Author:  bkupe

  ==============================================================================
*/

#include "MIDIManager.h"
#include "LGMLLogger.h"

juce_ImplementSingleton(MIDIManager)

MIDIManager::MIDIManager()
{
	
}

MIDIManager::~MIDIManager()
{
	
}

void MIDIManager::init()
{
	startTimer(500);
}

void MIDIManager::updateDeviceList(bool updateInput)
{
	StringArray deviceNames = updateInput ? MidiInput::getDevices() : MidiOutput::getDevices();
	
	StringArray sourceArray = updateInput ? inputDevices : outputDevices;

	StringArray devicesToAdd;
	StringArray devicesToRemove;

	//check added
	for (auto &newD : deviceNames.strings)
	{
		if (!sourceArray.contains(newD)) devicesToAdd.add(newD);
	}
	//check removed
	for (auto &sourceD : sourceArray.strings)
	{
		if (!deviceNames.contains(sourceD)) devicesToRemove.add(sourceD);
	}

	for (auto &d : devicesToAdd)
	{
		listeners.call(updateInput ? &MIDIManagerListener::midiInputAdded : &MIDIManagerListener::midiOutputAdded, d);
		LGMLLogger::getInstance()->logMessage("MIDI " + String(updateInput?"Input":"Output") + " Added : " + d);
	}

	for (auto &d : devicesToRemove)
	{
		listeners.call(updateInput ? &MIDIManagerListener::midiInputRemoved : &MIDIManagerListener::midiOutputRemoved, d);
		LGMLLogger::getInstance()->logMessage("MIDI " + String(updateInput ? "Input" : "Output") + " Removed : " + d);
	}


	if (updateInput) inputDevices = deviceNames;
	else outputDevices = deviceNames;
}

void MIDIManager::timerCallback()
{
	updateDeviceList(true); //update inputs
	updateDeviceList(false); //update outputs
}

