/*
  ==============================================================================

    MIDIControllerEditor.cpp
    Created: 11 May 2016 7:04:03pm
    Author:  bkupe

  ==============================================================================
*/

#include "MIDIControllerEditor.h"

MIDIControllerEditor::MIDIControllerEditor(MIDIControllerUI * controllerUI) :
	CustomEditor(controllerUI),
	midiController(controllerUI->midiController)
{

}

MIDIControllerEditor::~MIDIControllerEditor()
{
}
