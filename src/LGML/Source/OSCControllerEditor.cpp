/*
 ==============================================================================

 OSCControllerEditor.cpp
 Created: 10 May 2016 10:22:04am
 Author:  bkupe

 ==============================================================================
 */

#include "OSCControllerEditor.h"
#include "OSCControllerUI.h"

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
  speedLimit = oscController->speedLimit->createSlider();
  logOutOSC = oscController->logOutGoingOSC->createToggle();
  blockFeedback = oscController->blockFeedback->createToggle();

  sendAllTrigger = oscController->sendAllParameters->createBlinkUI();

  addAndMakeVisible(localPortUI);
  addAndMakeVisible(remoteHostUI);
  addAndMakeVisible(remotePortUI);
  addAndMakeVisible(logIncomingOSC);
  addAndMakeVisible(logOutOSC);
  addAndMakeVisible(sendAllTrigger);
  addAndMakeVisible(blockFeedback);
  addAndMakeVisible(speedLimit);

  addAndMakeVisible(innerContainer);
}

OSCControllerEditor::~OSCControllerEditor()
{
}

void OSCControllerEditor::resized()
{
  ControllerEditor::resized();

  Rectangle<int> r = getLocalBounds();

  r.removeFromTop(getVariablesHeight() + 30); //TODO : be removed when removing variable mechanism

  localPortUI->setBounds(r.removeFromTop(localPortUI->getHeight()));
  r.removeFromTop(2);
  remoteHostUI->setBounds(r.removeFromTop(remoteHostUI->getHeight()));
  remotePortUI->setBounds(r.removeFromTop(remotePortUI->getHeight()));
  r.removeFromTop(2);
  logIncomingOSC->setBounds(r.removeFromTop(15));
  r.removeFromTop(2);
  logOutOSC->setBounds(r.removeFromTop(15));
  r.removeFromTop(2);
  sendAllTrigger->setBounds(r.removeFromTop(15));
  r.removeFromTop(2);
  blockFeedback->setBounds(r.removeFromTop(15));
  r.removeFromTop(2);
  speedLimit->setBounds(r.removeFromTop(15));

  innerContainer.setBounds(r);

}

int OSCControllerEditor::getContentHeight()
{
//	const int lineHeight = 20;
	return ControllerEditor::getContentHeight() + 200;
}

void OSCControllerEditor::messageProcessed(const OSCMessage & , bool)
{
  
}
