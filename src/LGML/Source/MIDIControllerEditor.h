/*
  ==============================================================================

    MIDIControllerEditor.h
    Created: 11 May 2016 7:04:03pm
    Author:  bkupe

  ==============================================================================
*/

#ifndef MIDICONTROLLEREDITOR_H_INCLUDED
#define MIDICONTROLLEREDITOR_H_INCLUDED

#include "CustomEditor.h"
#include "MIDIControllerUI.h"
#include "MIDIUIHelper.h"

class MIDIControllerEditor : public CustomEditor
{
public:
	MIDIControllerEditor(MIDIControllerUI * controllerUI);
	virtual ~MIDIControllerEditor();

	MIDIDeviceChooser deviceChooser;

	MIDIController * midiController;

	void resized() override;
};


#endif  // MIDICONTROLLEREDITOR_H_INCLUDED
