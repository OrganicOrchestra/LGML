/*
  ==============================================================================

    MIDIControllerUI.h
    Created: 11 May 2016 7:03:58pm
    Author:  bkupe

  ==============================================================================
*/

#ifndef MIDICONTROLLERUI_H_INCLUDED
#define MIDICONTROLLERUI_H_INCLUDED

#include "MIDIController.h"
#include "ControllerUI.h"

class MIDIControllerUI : public  ControllerUI, public MIDIController::MIDIControllerListener
{
public:
	MIDIControllerUI(MIDIController * controller);
	virtual ~MIDIControllerUI();

	MIDIController * midiController;
	
	InspectorEditor * getEditor() override;

	void currentDeviceChanged(MIDIController *)override;
};

#endif  // MIDICONTROLLERUI_H_INCLUDED
