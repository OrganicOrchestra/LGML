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
class MIDIControllerUI;
class MIDIController;
#include "MIDIListener.h"
#include "MIDIUIHelper.h"
#include "JsEnvironmentUI.h"



class MIDIControllerEditor : public ControllerEditor, ComboBoxListener, MIDIListener::Listener
{
public:
	MIDIControllerEditor(MIDIControllerUI * controllerUI);
	virtual ~MIDIControllerEditor();

	MIDIController * midiController;
	JsEnvironmentUI jsUI;

	MIDIDeviceChooser deviceChooser;
	ScopedPointer<NamedControllableUI> channelStepper;
	ScopedPointer<BoolToggleUI> incomingToogle;

	 

	void resized() override;
	void comboBoxChanged(ComboBox *) override;

	int getContentHeight() override;

	void currentDeviceChanged(MIDIListener *) override;
};


#endif  // MIDICONTROLLEREDITOR_H_INCLUDED
