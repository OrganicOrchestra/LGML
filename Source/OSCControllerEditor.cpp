/* Copyright © Organic Orchestra, 2017
*
* This file is part of LGML.  LGML is a software to manipulate sound in realtime
*
* This program is free software; you can redistribute it and/or modify
* it under the terms of the GNU General Public License as published by
* the Free Software Foundation (version 3 of the License).
*
* This program is distributed in the hope that it will be useful,
* but WITHOUT ANY WARRANTY; without even the implied warranty of
* MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
*
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

  r.removeFromTop( 30);

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
