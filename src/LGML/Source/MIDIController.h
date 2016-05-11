/*
  ==============================================================================

    MIDIController.h
    Created: 2 Mar 2016 8:51:20pm
    Author:  bkupe

  ==============================================================================
*/

#ifndef MIDICONTROLLER_H_INCLUDED
#define MIDICONTROLLER_H_INCLUDED

#include "Controller.h"



class MIDIController : public Controller,public MidiInputCallback
{
public :
    MIDIController();

    void handleIncomingMidiMessage (MidiInput* source,
                                            const MidiMessage& message) override;

    void ListenToMidiPort(const String & );
private:

    String midiPortName;
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(MIDIController)
};


#endif  // MIDICONTROLLER_H_INCLUDED
