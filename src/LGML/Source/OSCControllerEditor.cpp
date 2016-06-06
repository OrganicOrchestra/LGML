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
	localPortUI->setNameLabelVisible(true);
	remoteHostUI = oscController->remoteHostParam->createStringParameterUI();
	remoteHostUI->setNameLabelVisible(true);
	remotePortUI = oscController->remotePortParam->createStringParameterUI();
	remotePortUI->setNameLabelVisible(true);

    logIncomingOSC = oscController->logIncomingOSC->createToggle();
    logOutOSC = oscController->logOutGoingOSC->createToggle();

	addAndMakeVisible(localPortUI);
	addAndMakeVisible(remoteHostUI);
	addAndMakeVisible(remotePortUI);
    addAndMakeVisible(logIncomingOSC);
    addAndMakeVisible(logOutOSC);


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
    logIncomingOSC->setBounds(r.removeFromTop(15));
	r.removeFromTop(2);
    logOutOSC->setBounds(r.removeFromTop(15));


	innerContainer.setBounds(r);

}

int OSCControllerEditor::getContentHeight()
{
	return localPortUI->getHeight() + remoteHostUI->getHeight() + remotePortUI->getHeight() + 60;
}

void OSCControllerEditor::messageProcessed(const OSCMessage & , bool)
{

}
