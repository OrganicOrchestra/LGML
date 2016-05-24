/*
  ==============================================================================

    MIDIControllerUI.cpp
    Created: 11 May 2016 7:03:58pm
    Author:  bkupe

  ==============================================================================
*/

#include "MIDIControllerUI.h"
#include "MIDIControllerEditor.h"
#include "MIDIController.h"


MIDIControllerUI::MIDIControllerUI(MIDIController * controller) :
	ControllerUI(controller),
	midiController(controller)
{
}


MIDIControllerUI::~MIDIControllerUI()
{
}

InspectorEditor * MIDIControllerUI::getEditor()
{
	return new MIDIControllerEditor(this);
}

void MIDIControllerUI::currentDeviceChanged(MIDIListener *)
{
	//nameTF->setSuffix(" [" + midiController->midiPortName + "]");
}

