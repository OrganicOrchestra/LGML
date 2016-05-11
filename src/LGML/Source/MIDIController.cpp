/*
 ==============================================================================

 MIDIController.cpp
 Created: 2 Mar 2016 8:51:20pm
 Author:  bkupe

 ==============================================================================
 */

#include "MIDIController.h"
#include "MIDIControllerUI.h"
#include "MIDIManager.h"
#include "DebugHelpers.h"

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
		MIDIManager::getInstance()->disableInputDevice(midiPortName);
        getAudioDeviceManager().removeMidiInputCallback(midiPortName, this);
    }

    midiPortName = deviceName;


    if (!midiPortName.isEmpty())
    {
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

    if(message.isController()){
        LOG("Incoming controlChange message : " + String(source->getName()) + " / " + String(message.getControllerValue()));
    }
    else if(message.isNoteOnOrOff()){
        LOG("Incoming note message : " + String(source->getName()) + " / " + String(message.getNoteNumber()) + " / "
            + (message.isNoteOn()?"on":"off"));
    }
    
    
}