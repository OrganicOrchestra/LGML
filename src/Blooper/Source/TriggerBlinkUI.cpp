/*
==============================================================================

TriggerBlinkUI.cpp
Created: 8 Mar 2016 3:45:53pm
Author:  bkupe

==============================================================================
*/

#include "../JuceLibraryCode/JuceHeader.h"
#include "TriggerBlinkUI.h"
#include "Style.h"
//==============================================================================
TriggerBlinkUI::TriggerBlinkUI(Trigger *t) :
	TriggerUI(t),
	blinkTime(200),
	refreshPeriod(50),
	intensity(0)
{
	setSize(30, 20);
	

	nameLabel.setJustificationType(Justification::centred);
	nameLabel.setText(t->niceName, NotificationType::dontSendNotification);
	addAndMakeVisible(nameLabel);
	nameLabel.setColour(Label::ColourIds::textColourId, TEXT_COLOR);
	Font f = nameLabel.getFont();
	f.setHeight(10);
	nameLabel.setFont(f);
	nameLabel.setInterceptsMouseClicks(false, false);
}

TriggerBlinkUI::~TriggerBlinkUI()
{
	
}

void TriggerBlinkUI::setTriggerReference(Trigger * t) {
	if (trigger != nullptr) {
		trigger->removeTriggerListener(this);
	}

	trigger = t;

	trigger->addTriggerListener(this);
}

void TriggerBlinkUI::triggerTriggered(Trigger * p) {
	startBlink();
}

void TriggerBlinkUI::paint(Graphics& g)
{
	g.setColour(NORMAL_COLOR.brighter(intensity));
	g.fillRoundedRectangle(getLocalBounds().toFloat(),2);
	
}


void TriggerBlinkUI::startBlink(){
	intensity = 1;
	startTimer(refreshPeriod);
}

void TriggerBlinkUI::timerCallback(){
	intensity-= refreshPeriod*1.0/blinkTime;
	if(intensity<0){
	intensity = 0;
	stopTimer();
	}
	repaint();
}


void TriggerBlinkUI::resized()
{
	// This method is where you should set the bounds of any child
	// components that your component contains..
	nameLabel.setBounds(getLocalBounds());

}

void TriggerBlinkUI::mouseDown(const MouseEvent& event) {
	trigger->trigger();

}

