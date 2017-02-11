/*
  ==============================================================================

    MIDIControllerEditor.cpp
    Created: 11 May 2016 7:04:03pm
    Author:  bkupe

  ==============================================================================
*/

#include "MIDIControllerEditor.h"
#include "MIDIController.h"

#include "MIDIListener.h"
#include "IntStepperUI.h"

MIDIControllerEditor::MIDIControllerEditor(Controller * _controller) :
	ControllerEditor(_controller,false),
	deviceChooser(true)
{
  ControllerEditor::hideVariableUIs = true;
  midiController = dynamic_cast<MIDIController*>( controller);
  jassert(midiController);
  jsUI = new JsEnvironmentUI(midiController->jsParameters);
	addAndMakeVisible(deviceChooser);
	deviceChooser.addListener(this);
	deviceChooser.setSelectedDevice(midiController->midiPortName,true);

    addAndMakeVisible(jsUI);

    incomingToogle = midiController->logIncoming->createToggle();
    addAndMakeVisible(incomingToogle);

	channelStepper = new NamedControllableUI(midiController->channelFilter->createStepper(),50);
	addAndMakeVisible(channelStepper);

	midiController->addMIDIListenerListener(this);
}

MIDIControllerEditor::~MIDIControllerEditor()
{
	midiController->removeMIDIListenerListener(this);
}

void MIDIControllerEditor::resized()
{
	ControllerEditor::resized();
  Rectangle<int> r = ControllerEditor::area;
	//r.removeFromTop(ControllerEditor::getContentHeight() + 10);
	deviceChooser.setBounds(r.removeFromTop(30));
	channelStepper->setBounds(r.removeFromTop(20));
	r.removeFromTop(5);
	incomingToogle->setBounds(r.removeFromTop(20));
  jsUI->setBounds(r.removeFromTop(40));
}

int MIDIControllerEditor::getContentHeight()
{
  return ControllerEditor::getContentHeight() + 140;
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
