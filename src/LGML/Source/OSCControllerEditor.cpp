/*
  ==============================================================================

    OSCControllerEditor.cpp
    Created: 10 May 2016 10:22:04am
    Author:  bkupe

  ==============================================================================
*/

#include "OSCControllerEditor.h"

OSCControllerEditor::OSCControllerEditor(OSCControllerUI * controllerUI) :
	ControllerEditor(controllerUI),
	oscController(controllerUI->oscController)
{

	localPortUI = oscController->localPortParam->createStringParameterUI();
	remoteHostUI = oscController->remoteHostParam->createStringParameterUI();
	remotePortUI = oscController->remotePortParam->createStringParameterUI();

	addAndMakeVisible(localPortUI);
	addAndMakeVisible(remoteHostUI);
	addAndMakeVisible(remotePortUI);
}

OSCControllerEditor::~OSCControllerEditor()
{
}

void OSCControllerEditor::resizedInternal(Rectangle<int> r)
{
	localPortUI->setBounds(r.removeFromTop(localPortUI->getHeight()));
	r.removeFromTop(10);
	remoteHostUI->setBounds(r.removeFromTop(remoteHostUI->getHeight()));
	r.removeFromTop(2);
	remotePortUI->setBounds(r.removeFromTop(remotePortUI->getHeight()));
	r.removeFromTop(10);

	resizedInternalOSC(r);

}

void OSCControllerEditor::resizedInternalOSC(Rectangle<int>)
{
	// to override by child classes
}

void OSCControllerEditor::messageProcessed(const OSCMessage & , bool)
{

}
