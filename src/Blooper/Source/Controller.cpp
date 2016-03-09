/*
  ==============================================================================

    Controller.cpp
    Created: 2 Mar 2016 8:49:50pm
    Author:  bkupe

  ==============================================================================
*/

#include "Controller.h"
#include "ControllerUI.h";

Controller::Controller(const String &name) :
	ControllableContainer(name)
{
	nameParam = addStringParameter("Name", name);
	nameParam->addListener(this);
	enabledParam = addBoolParameter("Enabled",true);
	enabledParam->addListener(this);
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

void Controller::parameterValueChanged(Parameter * p)
{
	if (p == nameParam) setNiceName(nameParam->value);
	else if (p == enabledParam)
	{
		DBG("set Enabled " + String(enabledParam->value));
	}
}
