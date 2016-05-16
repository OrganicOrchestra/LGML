/*
  ==============================================================================

    ControlVariableReference.cpp
    Created: 12 May 2016 4:04:14pm
    Author:  bkupe

  ==============================================================================
*/

#include "ControlVariableReference.h"

ControlVariableReference::ControlVariableReference() :
	ControllableContainer("reference"),
	referenceParam(nullptr),
	currentVariable(nullptr)
{
	alias = addStringParameter("Alias", "Alias of the reference.\nThis will be use in the conditions and consequences", "(alias)");
}

ControlVariableReference::~ControlVariableReference()
{
}

void ControlVariableReference::setCurrentVariable(ControlVariable * v)
{
	if (currentVariable == v) return;

	if (currentVariable != nullptr)
	{
		referenceParam = nullptr;
	}

	currentVariable = v;

	if (currentVariable != nullptr)
	{
		referenceParam = currentVariable->parameter;
		setNiceName("reference:" + v->parameter->niceName);
	}
	else
	{
		setNiceName("reference:none");
	}

	referenceListeners.call(&ControlVariableReferenceListener::currentReferenceChanged, this);
}

void ControlVariableReference::remove()
{
	referenceListeners.call(&ControlVariableReferenceListener::askForRemoveReference, this);
}
