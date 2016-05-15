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
#include "MidiListener.h"
#include "JsEnvironment.h"

class MIDIController : public Controller,public MIDIListener,public JsEnvironment
{
public :
    MIDIController();



    // should be implemented to build localenv
    void buildLocalEnv() override;
	void handleIncomingMidiMessage(MidiInput* source,
		const MidiMessage& message) override;

	ControllerUI * createUI() override;
    void    onContainerParameterChanged(Parameter * )override;

    static var sendCC(const var::NativeFunctionArgs & v);
    static var sendNoteOnFor(const var::NativeFunctionArgs & v);
    void     callJs(const MidiMessage& message);

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(MIDIController)
};


#endif  // MIDICONTROLLER_H_INCLUDED
