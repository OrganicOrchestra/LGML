/*
  ==============================================================================

    MIDIController.cpp
    Created: 2 Mar 2016 8:51:20pm
    Author:  bkupe

  ==============================================================================
*/

#include "MIDIController.h"


AudioDeviceManager & getAudioDeviceManager();

MIDIController::MIDIController()
  {
        }

void MIDIController::ListenToMidiPort(const juce::String & name){
    if(midiPortName!="")
       getAudioDeviceManager().removeMidiInputCallback(midiPortName, this);

    getAudioDeviceManager().addMidiInputCallback(midiPortName, this);

}

void MIDIController::handleIncomingMidiMessage (MidiInput* source,
                                                const MidiMessage& message) {

    
}