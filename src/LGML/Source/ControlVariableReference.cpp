/*
  ==============================================================================

    ControlVariableReference.cpp
    Created: 12 May 2016 4:04:14pm
    Author:  bkupe

  ==============================================================================
*/

#include "ControlVariableReference.h"
#include "Controller.h"
#include "ControlManager.h"

ControlVariableReference::ControlVariableReference() :
	ControllableContainer("reference"),
	currentVariable(nullptr)
{
	alias = addStringParameter("Alias", "Alias of the reference.\nThis will be use in the conditions and consequences", "(alias)");
}

ControlVariableReference::~ControlVariableReference()
{
	setCurrentVariable(nullptr);
}

var ControlVariableReference::getValue()
{
	if (currentVariable != nullptr) return currentVariable->parameter->value;
	return var();
}

void ControlVariableReference::setCurrentVariable(ControlVariable * v)
{

	if (currentVariable == v) return;

	if (currentVariable != nullptr)
	{
		currentVariable->removeControlVariableListener(this);
		currentVariable->parameter->removeParameterListener(this);
	}

	currentVariable = v;

	if (currentVariable != nullptr)
	{
		currentVariable->addControlVariableListener(this);
		currentVariable->parameter->addParameterListener(this);
		setNiceName("reference:" + currentVariable->parameter->niceName);
		if (alias->stringValue() == "(alias)") alias->setValue(currentVariable->parameter->shortName);
	}
	else
	{
		setNiceName("reference:none");
	}

	referenceListeners.call(&ControlVariableReferenceListener::referenceVariableChanged, this);
}

void ControlVariableReference::onContainerParameterChanged(Parameter * p)
{
	if (p == alias)
	{
		referenceListeners.call(&ControlVariableReferenceListener::referenceAliasChanged, this);
	}
}

void ControlVariableReference::parameterValueChanged(Parameter * p)
{
	if (currentVariable == nullptr) return;
	if (p == currentVariable->parameter)
	{
		referenceListeners.call(&ControlVariableReferenceListener::referenceValueChanged, this);
	}
	else
	{
		ControllableContainer::parameterValueChanged(p);
	}
}

void ControlVariableReference::remove()
{
	referenceListeners.call(&ControlVariableReferenceListener::askForRemoveReference, this);
}

var ControlVariableReference::getJSONData()
{
	var data = ControllableContainer::getJSONData();
	data.getDynamicObject()->setProperty("controllerName", currentVariable->controller->shortName);
	data.getDynamicObject()->setProperty("variableName", currentVariable->parameter->shortName);
	return data;
}

void ControlVariableReference::loadJSONDataInternal(var data)
{

	if (!data.getDynamicObject()->hasProperty("controllerName") || !data.getDynamicObject()->hasProperty("variableName")) return;

	String controllerName = data.getDynamicObject()->getProperty("controllerName").toString();
	String variableName = data.getDynamicObject()->getProperty("variableName").toString();

	ControllableContainer * cc = ControllerManager::getInstance()->getControllableContainerByName(controllerName);
	Controller * c = dynamic_cast<Controller *>(cc);
	if (c != nullptr)
	{
		DBG("No Controller found for : " << data.getDynamicObject()->getProperty("controllerName").toString());
		setCurrentVariable(c->getVariableForAddress("/" + data.getDynamicObject()->getProperty("variableName").toString()));
	}
}

void ControlVariableReference::variableRemoved(ControlVariable *)
{
	setCurrentVariable(nullptr);
}

void ControlVariableReference::variableNameChanged(ControlVariable *)
{
	referenceListeners.call(&ControlVariableReferenceListener::referenceVariableNameChanged, this);
}
