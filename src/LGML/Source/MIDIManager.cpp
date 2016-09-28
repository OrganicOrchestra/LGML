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
#include "DebugHelpers.h"

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

	if (updateInput) inputDevices = deviceNames;
	else outputDevices = deviceNames;

	for (auto &d : devicesToAdd)
	{
		listeners.call(updateInput ? &MIDIManagerListener::midiInputAdded : &MIDIManagerListener::midiOutputAdded, d);
		NLOG("MIDIManager", "MIDI " + String(updateInput?"Input":"Output") + " Added : " + d);
	}

	for (auto &d : devicesToRemove)
	{
		listeners.call(updateInput ? &MIDIManagerListener::midiInputRemoved : &MIDIManagerListener::midiOutputRemoved, d);
		NLOG("MIDIManager", "MIDI " + String(updateInput ? "Input" : "Output") + " Removed : " + d);
	}

	if (devicesToAdd.size() > 0 || devicesToRemove.size() > 0)
	{
		listeners.call(updateInput?&MIDIManagerListener::midiInputsChanged : &MIDIManagerListener::midiOutputsChanged);
	}

	
}

void MIDIManager::enableInputDevice(const String & deviceName)
{
	DeviceUsageCount * duc = getDUCForInputDeviceName(deviceName);
	DBG("MIDIManager  Enable Input device : " << deviceName << ", duc != null ?" << String(duc != nullptr));

	if (duc == nullptr)
	{
		duc = new DeviceUsageCount(deviceName);
		inputCounts.add(duc);
	}
	DBG("MIDIManager deviceCount before increment : " << duc->usageCount);

	duc->usageCount++;
	DBG("MIDIManager deviceCount after increment : " << duc->usageCount);
	if (duc->usageCount >= 1)
	{
		DBG("AudioDeviceManager:Enable Input device : " << duc->deviceName);
		getAudioDeviceManager().setMidiInputEnabled(duc->deviceName, true);
	}

	
}

MidiOutput * MIDIManager::enableOutputDevice(const String & deviceName)
{
	DeviceUsageCount * duc = getDUCForOutputDeviceName(deviceName);

	if (duc == nullptr)
	{
		duc = new DeviceUsageCount(deviceName);
		outputCounts.add(duc);
	}

	duc->usageCount++;

	//if (duc->usageCount == 1) getAudioDeviceManager().setMidiInputEnabled(duc->deviceName, true); //no output device handling ?

	StringArray inD = MidiOutput::getDevices();
	StringRef dRef(deviceName);
	MidiOutput * out = MidiOutput::openDevice(inD.indexOf(dRef));

  if(out){
    LOG("Midi Out opened : " << out->getName());
  }
  else {
    LOG("can't open MIDI device : " << deviceName);
  }
	return out;
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

// @Ben seems weird mixing duc
// btw do we really need that for output??
void MIDIManager::disableOutputDevice(const String & deviceName)
{
	DeviceUsageCount * duc = getDUCForOutputDeviceName(deviceName);
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
