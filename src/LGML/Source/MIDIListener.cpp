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
{
}

MIDIListener::~MIDIListener()
{
	midiPortName = String::empty;
}

void MIDIListener::setCurrentDevice(const String & deviceName)
{
    if (deviceName == midiPortName) return;

    if (!midiPortName.isEmpty())
    {
        MIDIManager::getInstance()->disableInputDevice(midiPortName);
		MIDIManager::getInstance()->disableOutputDevice(midiPortName);
        getAudioDeviceManager().removeMidiInputCallback(midiPortName, this);
    }

    midiPortName =  deviceName;


    if (!midiPortName.isEmpty())
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
		"MIDI Out is null";
		return;
	}

	//LOG("Send SysEx");

	MidiMessage msg = MidiMessage::createSysExMessage(data,dataCount);
	midiOutDevice->sendMessageNow(msg);
}
