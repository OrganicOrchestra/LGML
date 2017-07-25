/*
 ==============================================================================

 OSCControllerEditor.cpp
 Created: 10 May 2016 10:22:04am
 Author:  bkupe

 ==============================================================================
 */

#include "OSCControllerEditor.h"
#include "ParameterUIFactory.h"
#include "StringParameterUI.h"

OSCControllerEditor::OSCControllerEditor(Controller * _controller) :
ControllerEditor(_controller)

{
  oscController = dynamic_cast<OSCController*> (_controller);
  jassert(oscController);
  localPortUI = ParameterUIFactory::createDefaultUI(oscController->localPortParam);
  ((StringParameterUI*)localPortUI.get())->setNameLabelVisible(true);
  remoteHostUI = ParameterUIFactory::createDefaultUI(oscController->remoteHostParam);
  ((StringParameterUI*)remoteHostUI.get())->setNameLabelVisible(true);
  remotePortUI = ParameterUIFactory::createDefaultUI(oscController->remotePortParam);
  ((StringParameterUI*)remotePortUI.get())->setNameLabelVisible(true);

  logIncomingOSC = ParameterUIFactory::createDefaultUI(oscController->logIncomingOSC);
  speedLimit = ParameterUIFactory::createDefaultUI(oscController->speedLimit);
  logOutOSC = ParameterUIFactory::createDefaultUI(oscController->logOutGoingOSC);
  blockFeedback = ParameterUIFactory::createDefaultUI(oscController->blockFeedback);

  sendAllTrigger = ParameterUIFactory::createDefaultUI(oscController->sendAllParameters);

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
