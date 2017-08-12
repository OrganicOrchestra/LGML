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


#include "OSCDirectControllerContentUI.h"
#include "NodeManager.h"

OSCDirectControllerContentUI::OSCDirectControllerContentUI() :activityTrigger("O","Activity")
{
    activityTriggerUI = activityTrigger.createBlinkUI();

    addAndMakeVisible(activityTriggerUI);
    addAndMakeVisible(&activityLog);
    activityLog.setColour(activityLog.backgroundColourId,PANEL_COLOR.darker());
    activityLog.setColour(activityLog.textColourId, Colours::white);
    //activityLog.setEditable(true);
    activityLog.setJustificationType(Justification::bottomLeft);
    activityLog.setFont(activityLog.getFont().withHeight(12));
    //activityLog.setMultiLine(true, true);
}

OSCDirectControllerContentUI::~OSCDirectControllerContentUI()
{
    oscd->removeOSCDirectParameterListener(this);
}

void OSCDirectControllerContentUI::init()
{
    oscd = dynamic_cast<OSCDirectController*>(controller);

    oscd->addOSCDirectParameterListener(dynamic_cast<OSCDirectControllerContentUI*>(this));

    localPortUI = oscd->localPortParam->createStringParameterUI();
    remoteHostUI = oscd->remoteHostParam->createStringParameterUI();
    remotePortUI = oscd->remotePortParam->createStringParameterUI();

    addAndMakeVisible(localPortUI);
    addAndMakeVisible(remoteHostUI);
    addAndMakeVisible(remotePortUI);


    cui->setSize(getWidth(), 300);

    Array<Controllable *> nodeControllables = NodeManager::getInstance()->getAllControllables(true,false);
}

void OSCDirectControllerContentUI::resized()
{

    Rectangle<int> r = getLocalBounds().reduced(5);
    activityTriggerUI->setBounds(r.getRight() - 20, 0, 20, 20);

    localPortUI->setBounds(r.removeFromTop(localPortUI->getHeight()));//.withWidth(200));
    r.removeFromTop(10);
    remoteHostUI->setBounds(r.removeFromTop(remoteHostUI->getHeight()));//.withWidth(200));
    r.removeFromTop(2);
    remotePortUI->setBounds(r.removeFromTop(remotePortUI->getHeight()));//.withWidth(200));
    r.removeFromTop(10);
    activityLog.setBounds(r);

}

void OSCDirectControllerContentUI::mouseDown(const MouseEvent &)
{
}

void OSCDirectControllerContentUI::messageProcessed(const OSCMessage & msg, Result success)
{
    DBG("Success ? " + (success?"1":success.getErrorMessage()));

    String m = msg.getAddressPattern().toString() + " (" + (success ? "Success": success.getErrorMessage()) + ")";
    activityLines.add(m);
    if (activityLines.size() > 15) activityLines.remove(0);

    activityLog.setText(activityLines.joinIntoString("\n") , NotificationType::dontSendNotification);
    activityTrigger.trigger();
}
