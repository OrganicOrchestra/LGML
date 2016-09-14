/*
  ==============================================================================

    MIDIControllerEditor.h
    Created: 11 May 2016 7:04:03pm
    Author:  bkupe

  ==============================================================================
*/

#ifndef MIDICONTROLLEREDITOR_H_INCLUDED
#define MIDICONTROLLEREDITOR_H_INCLUDED

#include "ControllerEditor.h"
#include "MIDIControllerUI.h"
#include "MIDIUIHelper.h"
#include "JsEnvironmentUI.h"



class MIDIControllerEditor : public ControllerEditor, ComboBoxListener
{
public:
	MIDIControllerEditor(MIDIControllerUI * controllerUI);
	virtual ~MIDIControllerEditor();

	MIDIDeviceChooser deviceChooser;

	MIDIController * midiController;
    JsEnvironmentUI jsUI;
    ScopedPointer<BoolToggleUI> incomingToogle;
	void resized() override;
	void comboBoxChanged(ComboBox *) override;

	int getContentHeight() override;
};


#endif  // MIDICONTROLLEREDITOR_H_INCLUDED
