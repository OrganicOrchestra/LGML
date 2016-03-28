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

    enabledUI->setTopLeftPosition(5, 5);
    titleUI->setBounds(getLocalBounds().reduced(15,2));
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
    titleUI = controller->nameParam->createStringParameterUI();
    titleUI->setNameLabelVisible(false);
    addAndMakeVisible(titleUI);

    enabledUI = controller->enabledParam->createToggle();
    addAndMakeVisible(enabledUI);

}
