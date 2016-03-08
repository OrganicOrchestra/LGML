/*
  ==============================================================================

    ControllerContentUI.cpp
    Created: 8 Mar 2016 10:48:41pm
    Author:  bkupe

  ==============================================================================
*/

#include "ControllerContentUI.h"

ControllerContentUI::ControllerContentUI()
{
}

  void ControllerContentUI::setControllerAndUI(Controller * controller, ControllerUI * cui)
  {
	  this->controller = controller;
	  this->cui = cui;
	  init();
  }

  void ControllerContentUI::init()
  {
	  //to override
  }
