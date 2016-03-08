/*
  ==============================================================================

    ControllerHeaderUI.cpp
    Created: 8 Mar 2016 10:48:47pm
    Author:  bkupe

  ==============================================================================
*/

#include "ControllerHeaderUI.h"

ControllerHeaderUI::ControllerHeaderUI()
  {
	  setSize(40, 30);
  }

ControllerHeaderUI::~ControllerHeaderUI()
{
}

void ControllerHeaderUI::setControllerAndUI(Controller * controller, ControllerUI * cui)
{
	this->controller = controller;
	this->cui = cui;
}

void ControllerHeaderUI::init()
{
	//to override
}
