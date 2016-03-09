/*
  ==============================================================================

    ControllerUI.cpp
    Created: 8 Mar 2016 10:46:01pm
    Author:  bkupe

  ==============================================================================
*/

#include "ControllerUI.h"
#include "ControllerContentUI.h"
#include "ControllerHeaderUI.h"

ControllerUI::ControllerUI(Controller * controller, ControllerContentUI * contentUI, ControllerHeaderUI * headerUI) :
	controller(controller),
	contentUI(contentUI), headerUI(headerUI),
	ContourComponent(Colours::blue)
{
	if(this->headerUI == nullptr) this->headerUI = new ControllerHeaderUI();
	if (this->contentUI == nullptr) this->contentUI = new ControllerContentUI();


	this->headerUI->setControllerAndUI(controller, this);
	this->contentUI->setControllerAndUI(controller, this);

	addAndMakeVisible(this->headerUI);
	addAndMakeVisible(this->contentUI);

	if(getHeight() == 0) setSize(100,50);
}

ControllerUI::~ControllerUI()
{
}

void ControllerUI::paint(Graphics & g)
{
	ContourComponent::paint(g);

	g.setColour(PANEL_COLOR);
	g.fillRoundedRectangle(getLocalBounds().toFloat(), 4);
	g.setColour(PANEL_COLOR.darker());
	g.drawRoundedRectangle(getLocalBounds().toFloat(), 4, 2);
}

void ControllerUI::resized()
{
	Rectangle<int> r = getLocalBounds();
	headerUI->setBounds(r.removeFromTop(headerUI->getHeight()));
	contentUI->setBounds(r);
}


