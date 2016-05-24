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

    logIncomingOSC = oscController->logIncomingOSC->createToggle();


	addAndMakeVisible(localPortUI);
	addAndMakeVisible(remoteHostUI);
	addAndMakeVisible(remotePortUI);
    addAndMakeVisible(logIncomingOSC);

	addAndMakeVisible(innerContainer);
}

OSCControllerEditor::~OSCControllerEditor()
{
}

void OSCControllerEditor::resized()
{
	Rectangle<int> r = getLocalBounds();
	localPortUI->setBounds(r.removeFromTop(localPortUI->getHeight()));
	r.removeFromTop(10);
	remoteHostUI->setBounds(r.removeFromTop(remoteHostUI->getHeight()));
	r.removeFromTop(2);
	remotePortUI->setBounds(r.removeFromTop(remotePortUI->getHeight()));
	r.removeFromTop(2);
    logIncomingOSC->setBounds(r.removeFromTop(20));


	innerContainer.setBounds(r);

}

int OSCControllerEditor::getContentHeight()
{
	return localPortUI->getHeight() + remoteHostUI->getHeight() + remotePortUI->getHeight() + 30;
}

void OSCControllerEditor::messageProcessed(const OSCMessage & , bool)
{

}
