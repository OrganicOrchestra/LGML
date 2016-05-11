/*
 ==============================================================================

 MIDIController.cpp
 Created: 2 Mar 2016 8:51:20pm
 Author:  bkupe

 ==============================================================================
 */

#include "MIDIController.h"
#include "DebugHelpers.h"

#include "MIDIControllerUI.h"


AudioDeviceManager & getAudioDeviceManager();

MIDIController::MIDIController() :
	Controller("MIDI"),currentDeviceIdx(0)
{

    checkMidiDevices();
    listenToMidiPort(0);

}

void MIDIController::listenToMidiPort(int idx){
    if(StringRef curName = getDeviceName(currentDeviceIdx)){
        getAudioDeviceManager().setMidiInputEnabled(curName,false);
        getAudioDeviceManager().removeMidiInputCallback(curName, this);
    }

    currentDeviceIdx = idx;
    if(StringRef curName = getDeviceName(currentDeviceIdx)){
        getAudioDeviceManager().setMidiInputEnabled(curName,true);
        getAudioDeviceManager().addMidiInputCallback(curName, this);
    }

}

void MIDIController::checkMidiDevices(){midiDevices = MidiInput::getDevices();}


StringRef MIDIController::getDeviceName(int idx){
    if(idx>=0 && idx <midiDevices.size())
        return midiDevices.getReference(idx);
    else
        return StringRef("");
}

ControllerUI * MIDIController::createUI()
{
	return new MIDIControllerUI(this);
}

void MIDIController::handleIncomingMidiMessage (MidiInput* source,
                                                const MidiMessage& message) 
{
    if(message.isController()){
	LOG("Incoming midi message : " + String(source->getName()) + " / " + String(message.getControllerValue()));
    }
    else if(message.isNoteOnOrOff()){
LOG("Incoming midi message : " + String(source->getName()) + " / " + String(message.getNoteNumber()) + " / "
    + (message.isNoteOn()?"on":"off"));
    }

}