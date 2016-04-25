/*
  ==============================================================================

    OSCDirectControllerContentUI.cpp
    Created: 8 Mar 2016 10:47:26pm
    Author:  bkupe

  ==============================================================================
*/

#include "OSCDirectControllerContentUI.h"
#include "NodeManager.h"

OSCDirectControllerContentUI::OSCDirectControllerContentUI() :activityTrigger("O","Activity")
{
	activityTriggerUI = activityTrigger.createBlinkUI();
	
	addAndMakeVisible(activityTriggerUI);
	addAndMakeVisible(&activityLog);
	activityLog.setColour(activityLog.backgroundColourId,Colours::white);
	activityLog.setEditable(true);
	activityLog.setJustificationType(Justification::bottomLeft);
}

OSCDirectControllerContentUI::~OSCDirectControllerContentUI()
{
	oscd->removeOSCDirectParameterListener(this);
}

void OSCDirectControllerContentUI::init()
{
    oscd = (OSCDirectController *)controller;

	oscd->addOSCDirectParameterListener(this);

    localPortUI = oscd->localPortParam->createStringParameterUI();
    remoteHostUI = oscd->remoteHostParam->createStringParameterUI();
    remotePortUI = oscd->remotePortParam->createStringParameterUI();

    addAndMakeVisible(localPortUI);
    addAndMakeVisible(remoteHostUI);
    addAndMakeVisible(remotePortUI);


    cui->setSize(300, 300);

    Array<Controllable *> nodeControllables = NodeManager::getInstance()->getAllControllables(true,false);

    /*
    for (auto &_c : nodeControllables)
    {
        _c;
        DBG(_c->controlAddress+"     "+_c->description);
    }
    */
}

void OSCDirectControllerContentUI::resized()
{

    Rectangle<int> r = getLocalBounds().reduced(5);
	activityTriggerUI->setBounds(r.getRight() - 20, 0, 20, 20);

    localPortUI->setBounds(r.removeFromTop(localPortUI->getHeight()));
    r.removeFromTop(10);
    remoteHostUI->setBounds(r.removeFromTop(remoteHostUI->getHeight()));
    r.removeFromTop(2);
    remotePortUI->setBounds(r.removeFromTop(remotePortUI->getHeight()));
	r.removeFromTop(10);
	activityLog.setBounds(r);

}

void OSCDirectControllerContentUI::mouseDown(const MouseEvent &)
{
}

void OSCDirectControllerContentUI::messageProcessed(const OSCMessage & msg, bool success)
{
	activityLog.setText(activityLog.getText() + String("\n") + msg.getAddressPattern().toString(),NotificationType::dontSendNotification);
	activityTrigger.trigger();
}
