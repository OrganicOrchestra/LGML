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
	activityTrigger("Activity","OSC Activity indicator"),
	oscController(controllerUI->oscController)
{

	activityTriggerUI = activityTrigger.createBlinkUI();

	addAndMakeVisible(activityTriggerUI);
	addAndMakeVisible(&activityLog);
	activityLog.setColour(activityLog.backgroundColourId, PANEL_COLOR.darker());
	activityLog.setColour(activityLog.textColourId, Colours::white);

	activityLog.setJustificationType(Justification::bottomLeft);
	activityLog.setFont(activityLog.getFont().withHeight(12));

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
	activityTriggerUI->setBounds(r.getRight() - 20, 0, 20, 20);

	localPortUI->setBounds(r.removeFromTop(localPortUI->getHeight()));
	r.removeFromTop(10);
	remoteHostUI->setBounds(r.removeFromTop(remoteHostUI->getHeight()));
	r.removeFromTop(2);
	remotePortUI->setBounds(r.removeFromTop(remotePortUI->getHeight()));
	r.removeFromTop(10);
	activityLog.setBounds(r.removeFromBottom(150));

	resizedInternalOSC(r);

}

void OSCControllerEditor::resizedInternalOSC(Rectangle<int>)
{
	// to override by child classes
}

void OSCControllerEditor::messageProcessed(const OSCMessage & msg, bool success)
{
	String m = msg.getAddressPattern().toString() + " (" + (success ? "Success" : "Failed") + ")";
	activityLines.add(m);
	if (activityLines.size() > 15) activityLines.remove(0);

	activityLog.setText(activityLines.joinIntoString("\n"), NotificationType::dontSendNotification);
	activityTrigger.trigger();
}
