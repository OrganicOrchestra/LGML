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

	String midiPortName;

	void setCurrentDevice(const String &deviceName);
	
	void handleIncomingMidiMessage(MidiInput* source,
		const MidiMessage& message) override;

	ControllerUI * createUI() override;


	class  MIDIControllerListener
	{
	public:
		/** Destructor. */
		virtual ~MIDIControllerListener() {}
		virtual void currentDeviceChanged(MIDIController *) {}


	};

	ListenerList<MIDIControllerListener> midiControllerListeners;
	void addMIDIControllerListener(MIDIControllerListener* newListener) { midiControllerListeners.add(newListener); }
	void removeMIDIControllerListener(MIDIControllerListener* listener) { midiControllerListeners.remove(listener); }


    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(MIDIController)
};


#endif  // MIDICONTROLLER_H_INCLUDED
