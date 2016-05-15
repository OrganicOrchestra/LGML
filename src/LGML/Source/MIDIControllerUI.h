/*
  ==============================================================================

    MIDIControllerUI.h
    Created: 11 May 2016 7:03:58pm
    Author:  bkupe

  ==============================================================================
*/

#ifndef MIDICONTROLLERUI_H_INCLUDED
#define MIDICONTROLLERUI_H_INCLUDED


#include "ControllerUI.h"
#include "MIDIListener.h"

class MIDIController;

class MIDIControllerUI : public  ControllerUI, public MIDIListener::Listener
{
public:
	MIDIControllerUI(MIDIController * controller);
	virtual ~MIDIControllerUI();

	MIDIController * midiController;
	
	InspectorEditor * getEditor() override;

	void currentDeviceChanged(MIDIListener *)override;
};

#endif  // MIDICONTROLLERUI_H_INCLUDED
