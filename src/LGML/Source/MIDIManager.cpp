/*
  ==============================================================================

    MIDIManager.cpp
    Created: 11 May 2016 7:17:05pm
    Author:  bkupe

  ==============================================================================
*/

#include "MIDIManager.h"
#include "LGMLLogger.h"
#include "MainComponent.h"

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

void MIDIManager::enableInputDevice(const String & deviceName)
{
	//DBG("MIDIManager  Enable Input device : " << deviceName);
	DeviceUsageCount * duc = getDUCForInputDeviceName(deviceName);

	if (duc == nullptr)
	{
		duc = new DeviceUsageCount(deviceName);
		inputCounts.add(duc);
	}

	duc->usageCount++;
	if (duc->usageCount == 1)
	{
		//DBG("AudioDeviceManager:Enable Input device : " << duc->deviceName);
		getAudioDeviceManager().setMidiInputEnabled(duc->deviceName, true);
	}
}

void MIDIManager::enableOutputDevice(const String & deviceName)
{
	DeviceUsageCount * duc = getDUCForOutputDeviceName(deviceName);

	if (duc == nullptr)
	{
		duc = new DeviceUsageCount(deviceName);
		outputCounts.add(duc);
	}

	duc->usageCount++;
	//if (duc->usageCount == 1) getAudioDeviceManager().setMidiInputEnabled(duc->deviceName, true); //no output device handling ?
}

void MIDIManager::disableInputDevice(const String & deviceName)
{
	DeviceUsageCount * duc = getDUCForInputDeviceName(deviceName);
	if (duc == nullptr) return;
	duc->usageCount--;
	if (duc->usageCount == 0)
	{
		DBG("Disable Input device : " << duc->deviceName);
		getAudioDeviceManager().setMidiInputEnabled(duc->deviceName, false);
	}
}

void MIDIManager::disableOutputDevice(const String & deviceName)
{
	DeviceUsageCount * duc = getDUCForInputDeviceName(deviceName);
	if (duc == nullptr) return;
	duc->usageCount--;
}

MIDIManager::DeviceUsageCount * MIDIManager::getDUCForInputDeviceName(const String & deviceName)
{
	for (auto &duc : inputCounts)
	{
		if (duc->deviceName == deviceName) return duc;
	}
	return nullptr;
}

MIDIManager::DeviceUsageCount * MIDIManager::getDUCForOutputDeviceName(const String & deviceName)
{
	for (auto &duc : outputCounts)
	{
		if (duc->deviceName == deviceName) return duc;
	}
	return nullptr;
}

void MIDIManager::timerCallback()
{
	updateDeviceList(true); //update inputs
	updateDeviceList(false); //update outputs
}

