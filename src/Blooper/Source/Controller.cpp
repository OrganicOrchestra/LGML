/*
  ==============================================================================

    Controller.cpp
    Created: 2 Mar 2016 8:49:50pm
    Author:  bkupe

  ==============================================================================
*/

#include "Controller.h"
#include "ControllerUI.h";

Controller::Controller()
  {
  }

  Controller::~Controller()
  {
  }

  ControllerUI * Controller::createUI()
  {
	  return new ControllerUI(this);
  }

  void Controller::remove()
  {
	  listeners.call(&Listener::askForRemoveController, this);
  }
