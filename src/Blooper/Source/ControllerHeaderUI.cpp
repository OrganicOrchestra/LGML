/*
  ==============================================================================

    ControllerHeaderUI.cpp
    Created: 8 Mar 2016 10:48:47pm
    Author:  bkupe

  ==============================================================================
*/

#include "ControllerHeaderUI.h"

ControllerHeaderUI::ControllerHeaderUI() :ContourComponent(Colours::green)
  {
	  setSize(40, 30);
  }

ControllerHeaderUI::~ControllerHeaderUI()
{
}

void ControllerHeaderUI::resized()
{
	if (enabledUI == nullptr) return;

	
	DBG("header resized : "+getLocalBounds().toString());
	enabledUI->setTopLeftPosition(5, 5);
	titleLabel.setBounds(getLocalBounds().reduced(15,0));
}

void ControllerHeaderUI::setControllerAndUI(Controller * controller, ControllerUI * cui)
{
	this->controller = controller;
	this->cui = cui;
	init();
}

void ControllerHeaderUI::init()
{
	//to override
	titleLabel.setJustificationType(Justification::topLeft);
	titleLabel.setText(controller->name,NotificationType::dontSendNotification);
	addAndMakeVisible(titleLabel);

	enabledUI = controller->enabledParam->createToggle();
	addAndMakeVisible(enabledUI);

}
