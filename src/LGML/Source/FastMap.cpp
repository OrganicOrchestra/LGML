/*
  ==============================================================================

    FastMap.cpp
    Created: 17 May 2016 6:05:27pm
    Author:  bkupe

  ==============================================================================
*/

#include "FastMap.h"

FastMap::FastMap() :
	reference(nullptr),
	target(nullptr),
	ControllableContainer("FastMap")
{
	reference = new ControlVariableReference();
	reference->addReferenceListener(this);

	minInputVal = addFloatParameter("In Min", "Minimum Input Value", 0, 0, 1);
	maxInputVal = addFloatParameter("In Max", "Maximum Input Value", 1, 0, 1);
	minOutputVal = addFloatParameter("Out Min", "Minimum Output Value", 0, 0, 1);
	maxOutputVal = addFloatParameter("Out Max", "Maximum Output Value", 1, 0, 1);
}

FastMap::~FastMap()
{

	setReference(nullptr);
	setTarget(nullptr);

	fastMapListeners.call(&FastMapListener::fastMapRemoved, this);
}

void FastMap::process()
{
	float sourceVal = (float)reference->getValue();
	if (target == nullptr) return;
	
	bool newIsInRange = (sourceVal >= minInputVal->floatValue() && sourceVal < maxInputVal->floatValue());

	if (target->type == Controllable::TRIGGER)
	{
		if (newIsInRange != isInRange && isInRange) ((Trigger*)target)->trigger();
	}
	else
	{
		if (target->type == Controllable::BOOL)
		{
			((BoolParameter *)target)->setValue(newIsInRange);
		}else
		{
			float targetVal = jmap<float>(sourceVal, minInputVal->floatValue(), maxInputVal->floatValue(), minOutputVal->floatValue(), maxOutputVal->floatValue());
			targetVal = jlimit<float>(minOutputVal->floatValue(), maxOutputVal->floatValue(),targetVal);
			((Parameter *)target)->setValue(targetVal);

		}
	}

	isInRange = newIsInRange;
}

void FastMap::setReference(ControlVariableReference * r)
{
	if (reference == r) return;
	if (reference != nullptr)
	{
		reference->removeReferenceListener(this);
	}
	reference = r;
	if (reference != nullptr)
	{
		reference->addReferenceListener(this);
	}

	fastMapListeners.call(&FastMapListener::fastMapReferenceChanged, this);
}

void FastMap::setTarget(Controllable * c)
{
	DBG("Set Target !");
	if (target == c) return;

	if (target != nullptr)
	{
		target->removeControllableListener(this);
	}
	target = c;

	if (target != nullptr)
	{
		target->addControllableListener(this);
	}

	fastMapListeners.call(&FastMapListener::fastMapTargetChanged, this);
}

void FastMap::remove()
{
	fastMapListeners.call(&FastMapListener::askForRemoveFastMap, this);
}

void FastMap::referenceValueChanged(ControlVariableReference *)
{
	process();
}

void FastMap::controllableRemoved(Controllable * c)
{
	if (c == target)
	{
		setTarget(nullptr);
	}
}
