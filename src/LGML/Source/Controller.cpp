/*
  ==============================================================================

    Controller.cpp
    Created: 2 Mar 2016 8:49:50pm
    Author:  bkupe

  ==============================================================================
*/

#include "Controller.h"
#include "ControllerUI.h"
#include "ControllerFactory.h"

Controller::Controller(const String &name) :
    ControllableContainer(name)
{
    nameParam = addStringParameter("Name", "Set the name of the controller.",name);
    enabledParam = addBoolParameter("Enabled","Set whether the controller is enabled or disabled", true);

    controllerTypeEnum = 0;//init
}


Controller::~Controller()
{
    DBG("Remove Controller");
}

ControllerUI * Controller::createUI()
{
    return new ControllerUI(this);
}

var Controller::getJSONData()
{
	var data = ControllableContainer::getJSONData();
    data.getDynamicObject()->setProperty("controllerType", ControllerFactory::controllerToString(this));

    return data;
}

void Controller::loadJSONDataInternal(var data)
{
	//params taken care in ControllableContainer::loadJSONData
}

void Controller::remove()
{
    listeners.call(&Controller::Listener::askForRemoveController, this);
}

void Controller::onContainerParameterChanged(Parameter * p)
{

    if (p == nameParam) setNiceName(nameParam->value);
    else if (p == enabledParam)
    {
        DBG("set Controller Enabled " + String(enabledParam->value));
    }
}
