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
    oscd = (OSCDirectController *)controller;

	oscd->addOSCDirectParameterListener(this);

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

    localPortUI->setBounds(r.removeFromTop(localPortUI->getHeight()).withWidth(200));
    r.removeFromTop(10);
    remoteHostUI->setBounds(r.removeFromTop(remoteHostUI->getHeight()).withWidth(200));
    r.removeFromTop(2);
    remotePortUI->setBounds(r.removeFromTop(remotePortUI->getHeight()).withWidth(200));
	r.removeFromTop(10);
	activityLog.setBounds(r);

}

void OSCDirectControllerContentUI::mouseDown(const MouseEvent &)
{
}

void OSCDirectControllerContentUI::messageProcessed(const OSCMessage & msg, bool success)
{
	DBG("Success ? " + String(success));
	
	String m = msg.getAddressPattern().toString() + " (" + (success ? "Success" : "Failed") + ")";
	activityLines.add(m);
	if (activityLines.size() > 15) activityLines.remove(0);

	activityLog.setText(activityLines.joinIntoString("\n") , NotificationType::dontSendNotification);
	activityTrigger.trigger();
}
