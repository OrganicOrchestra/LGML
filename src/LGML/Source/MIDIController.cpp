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

void MIDIController::ListenToMidiPort(const juce::String & name){
    if(midiPortName!="")
       getAudioDeviceManager().removeMidiInputCallback(midiPortName, this);

    getAudioDeviceManager().addMidiInputCallback(midiPortName, this);

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