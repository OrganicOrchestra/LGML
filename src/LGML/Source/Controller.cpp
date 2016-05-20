/*
  ==============================================================================

    Controller.cpp
    Created: 2 Mar 2016 8:49:50pm
    Author:  bkupe

  ==============================================================================
*/

#include "Controller.h"
#include "ControllerFactory.h"
#include "ControllerUI.h"
#include "ControlVariable.h"
#include "DebugHelpers.h"


const Identifier Controller::controllerTypeIdentifier("controllerType");

Controller::Controller(const String &_name) :
	ControllableContainer(_name)
{
    nameParam = addStringParameter("Name", "Set the name of the controller.", _name);
    enabledParam = addBoolParameter("Enabled","Set whether the controller is enabled or disabled", true);

    controllerTypeEnum = 0; //init
}


Controller::~Controller()
{
    DBG("Remove Controller");
}

var Controller::getJSONData()
{
	var data = ControllableContainer::getJSONData();
    data.getDynamicObject()->setProperty(controllerTypeIdentifier, ControllerFactory::controllerToString(this));

    return data;
}

void Controller::loadJSONDataInternal(var data)
{
}


String Controller::getUniqueVariableNameFor(const String & baseName, int index)
{
	String tName = baseName + " " + String(index);
	for (auto & v : variables)
	{
		if (v->parameter->niceName == tName)
		{
			return getUniqueVariableNameFor(baseName, index + 1);
		}
	}

	return tName;
}

ControllerUI * Controller::createUI()
{
	return new ControllerUI(this);
}

void Controller::addVariable(Parameter * p)
{
	ControlVariable * v = new ControlVariable(this, p);
	p->replaceSlashesInShortName = false;
	variables.add(v);
	v->addControlVariableListener(this);
	controllerListeners.call(&ControllerListener::variableAdded, this, v);
}

void Controller::removeVariable(ControlVariable * v)
{
	NLOG("Controller", "removeVariable");
	v->removeControlVariableListener(this);
	controllerListeners.call(&ControllerListener::variableRemoved, this, v);
	variables.removeObject(v);
}

ControlVariable * Controller::getVariableForAddress(const String & address)
{
	for (auto &v : variables)
	{
		if (v->parameter->controlAddress == address) return v;
	}

	return nullptr;
}

void Controller::remove()
{
	controllerListeners.call(&ControllerListener::askForRemoveController, this);
}

void Controller::onContainerParameterChanged(Parameter * p)
{
	if (p == nameParam)
	{
		setNiceName(nameParam->stringValue());
	}
    else if (p == enabledParam)
    {
       // DBG("set Controller Enabled " + String(enabledParam->boolValue()));
    }
}

void Controller::askForRemoveVariable(ControlVariable * variable)
{
	removeVariable(variable);
}
