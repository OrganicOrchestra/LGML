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

  void ControllerContentUI::setControllerAndUI(Controller * _controller, ControllerUI * _cui)
  {
      this->controller = _controller;
      this->cui = _cui;
      init();
  }

  void ControllerContentUI::init()
  {
      //to override
  }
