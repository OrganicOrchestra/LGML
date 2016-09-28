/*
  ==============================================================================

    MIDIControllerEditor.cpp
    Created: 11 May 2016 7:04:03pm
    Author:  bkupe

  ==============================================================================
*/

#include "MIDIControllerEditor.h"
#include "MIDIController.h"
#include "MIDIControllerUI.h"
#include "MIDIListener.h"

MIDIControllerEditor::MIDIControllerEditor(MIDIControllerUI * controllerUI) :
	ControllerEditor(controllerUI),
	midiController(controllerUI->midiController),
	deviceChooser(true),
    jsUI(controllerUI->midiController)
{
	addAndMakeVisible(deviceChooser);
	deviceChooser.addListener(this);
	deviceChooser.setSelectedDevice(midiController->midiPortName,true);

    addAndMakeVisible(jsUI);

    incomingToogle = midiController->logIncoming->createToggle();
    addAndMakeVisible(incomingToogle);

	midiController->addMIDIListenerListener(this);
}

MIDIControllerEditor::~MIDIControllerEditor()
{
	midiController->removeMIDIListenerListener(this);
}

void MIDIControllerEditor::resized()
{
	//ControllerEditor::resized();
	Rectangle<int> r = getLocalBounds();
	//r.removeFromTop(ControllerEditor::getContentHeight() + 10);
	deviceChooser.setBounds(r.removeFromTop(30));
    incomingToogle->setBounds(r.removeFromTop(20));
    jsUI.setBounds(r.removeFromTop(20));
}

int MIDIControllerEditor::getContentHeight()
{
	return 100;
}

void MIDIControllerEditor::currentDeviceChanged(MIDIListener *)
{
	DBG("Current device changed : " << midiController->midiPortName);
	deviceChooser.setSelectedDevice(midiController->midiPortName, true);
}

void MIDIControllerEditor::comboBoxChanged(ComboBox *cb)
{
	if (cb == &deviceChooser)
	{
		if (deviceChooser.getSelectedItemIndex() > 0)
		{
			midiController->deviceInName->setValue(deviceChooser.getItemText(deviceChooser.getSelectedItemIndex()));
		}
	}
}
