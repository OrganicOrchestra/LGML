/*
  ==============================================================================

    MIDIControllerEditor.cpp
    Created: 11 May 2016 7:04:03pm
    Author:  bkupe

  ==============================================================================
*/

#include "MIDIControllerEditor.h"
#include "MIDIController.h"

MIDIControllerEditor::MIDIControllerEditor(MIDIControllerUI * controllerUI) :
	CustomEditor(controllerUI),
	midiController(controllerUI->midiController),
	deviceChooser(true),
    jsUI(controllerUI->midiController)
{
	addAndMakeVisible(deviceChooser);
	deviceChooser.addListener(this);

    addAndMakeVisible(jsUI);
}

MIDIControllerEditor::~MIDIControllerEditor()
{
}

void MIDIControllerEditor::resized()
{
	Rectangle<int> r = getLocalBounds();
	deviceChooser.setBounds(r.removeFromTop(20));
    jsUI.setBounds(r.removeFromTop(20));
}

void MIDIControllerEditor::comboBoxChanged(ComboBox *cb)
{
	if (cb == &deviceChooser)
	{
		if (deviceChooser.getSelectedItemIndex() > 0)
		{
			midiController->setCurrentDevice(deviceChooser.getItemText(deviceChooser.getSelectedItemIndex()));
		}
	}
}
