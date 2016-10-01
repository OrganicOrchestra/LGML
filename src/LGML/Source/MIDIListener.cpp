/*
  ==============================================================================

    MIDIListener.cpp
    Created: 12 May 2016 9:42:56am
    Author:  Martin Hermant

  ==============================================================================
*/

#include "MIDIListener.h"
#include "DebugHelpers.h"
#include "MainComponent.h"

MIDIListener::MIDIListener()
{midiPortName = String::empty;
    MIDIManager::getInstance()->addMIDIListener(this);
}

MIDIListener::~MIDIListener()
{
        MIDIManager::getInstance()->removeMIDIListener(this);
	midiPortName = String::empty;
}

void MIDIListener::setCurrentDevice(const String & deviceName)
{
	//if (deviceName == midiPortName) return;

	if (deviceName.isNotEmpty())
	{
		if (MIDIManager::getInstance()->inputDevices.indexOf(deviceName) == -1)
		{
			ghostPortName = deviceName;
			setCurrentDevice(String::empty);
			return;
		}
	}

    if (midiPortName.isNotEmpty())
    {
        MIDIManager::getInstance()->disableInputDevice(midiPortName);
		MIDIManager::getInstance()->disableOutputDevice(midiPortName);
        getAudioDeviceManager().removeMidiInputCallback(midiPortName, this);
    }

	

	midiPortName = deviceName;


    if (midiPortName.isNotEmpty())
    {
		MIDIManager::getInstance()->enableInputDevice(midiPortName);
		midiOutDevice = MIDIManager::getInstance()->enableOutputDevice(midiPortName);
        getAudioDeviceManager().addMidiInputCallback(midiPortName, this);
    }

    MIDIListenerListeners.call(&MIDIListener::Listener::currentDeviceChanged, this);
}

void MIDIListener::sendNoteOn(int channel, int pitch, int velocity)
{
	if (midiOutDevice == nullptr)
	{
		LOG("MIDI Out is null");
		return;
	}

	//LOG("Send Note On");

	MidiMessage msg = MidiMessage::noteOn(channel, pitch, (uint8)velocity);
	midiOutDevice->sendMessageNow(msg);
}

void MIDIListener::sendNoteOff(int channel, int pitch, int velocity)
{
	if (midiOutDevice == nullptr)
	{
		LOG("MIDI Out is null");
		return;
	}

	//LOG("Send Note Off");


	MidiMessage msg = MidiMessage::noteOff(channel, pitch, (uint8)velocity);
	midiOutDevice->sendMessageNow(msg);
}

void MIDIListener::sendCC(int channel, int number, int value)
{
	if (midiOutDevice == nullptr)
	{
		LOG("MIDI Out is null");
		return;
	}

	//LOG("Send CC");


	MidiMessage msg = MidiMessage::controllerEvent(channel, number, value);
	midiOutDevice->sendMessageNow(msg);
}

void MIDIListener::sendSysEx(uint8 * data, int dataCount)
{
	if (midiOutDevice == nullptr)
	{
		DBG("MIDI Out is null");
		return;
	}

	//LOG("Send SysEx");

	MidiMessage msg = MidiMessage::createSysExMessage(data,dataCount);
	midiOutDevice->sendMessageNow(msg);
}

void MIDIListener::midiInputAdded(String & s) 
{
	DBG("MIDIListener :: inputAdded " << s << ",portName = " << midiPortName << ", ghost = " << ghostPortName);

	if (s == midiPortName)
	{
		setCurrentDevice(midiPortName);
		ghostPortName = String::empty;
	} else if (s == ghostPortName)
	{
		setCurrentDevice(ghostPortName);
		ghostPortName = String::empty;
	}
}

void MIDIListener::midiInputRemoved(String & s)
{
	if (s == midiPortName)
	{
		ghostPortName = s;
		setCurrentDevice(String::empty);
	}
}

void MIDIListener::midiOutputAdded(String & s) {
	if (s == midiPortName)
	{
		setCurrentDevice(midiPortName);
	}
}

void MIDIListener::midiOutputRemoved(String & /*s*/)
{
	//
}
