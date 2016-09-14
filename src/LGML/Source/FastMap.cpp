/*
  ==============================================================================

    FastMap.cpp
    Created: 17 May 2016 6:05:27pm
    Author:  bkupe

  ==============================================================================
*/

#include "FastMap.h"
#include "NodeManager.h"

FastMap::FastMap() :
	reference(nullptr),
	target(nullptr),
	ControllableContainer("FastMap")
{
	reference = new ControlVariableReference();
	reference->addReferenceListener(this);

	enabledParam = addBoolParameter("Enabled", "Enabled / Disable Fast Map", true);

	minInputVal = addFloatParameter("In Min", "Minimum Input Value", 0, 0, 1);
	maxInputVal = addFloatParameter("In Max", "Maximum Input Value", 1, 0, 1);
	minOutputVal = addFloatParameter("Out Min", "Minimum Output Value", 0, 0, 1);
	maxOutputVal = addFloatParameter("Out Max", "Maximum Output Value", 1, 0, 1);

	invertParam = addBoolParameter("Invert", "Invert the output signal", false);

}

FastMap::~FastMap()
{

	setReference(nullptr);
	setTarget(nullptr);

	fastMapListeners.call(&FastMapListener::fastMapRemoved, this);
}

void FastMap::process()
{
	if (!enabledParam->boolValue()) return;

	float sourceVal = (float)reference->getValue();
	if (target == nullptr) return;

	bool newIsInRange = (sourceVal >= minInputVal->floatValue() && sourceVal <= maxInputVal->floatValue());

	if (invertParam->boolValue()) newIsInRange = !newIsInRange;

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
			if (minOutputVal->floatValue() < maxOutputVal->floatValue())
			{
				float targetVal = juce::jmap<float>(sourceVal, minInputVal->floatValue(), maxInputVal->floatValue(), minOutputVal->floatValue(), maxOutputVal->floatValue());
				targetVal = juce::jlimit<float>(minOutputVal->floatValue(), maxOutputVal->floatValue(), targetVal);
				if (invertParam->boolValue()) targetVal = maxOutputVal->floatValue() - (targetVal - minOutputVal->floatValue());
				((Parameter *)target)->setNormalizedValue(targetVal);
			}
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

var FastMap::getJSONData()
{
	var data = ControllableContainer::getJSONData();
	if (reference != nullptr)
	{
		data.getDynamicObject()->setProperty("reference", reference->getJSONData());
	}

	if (target != nullptr)
	{
		data.getDynamicObject()->setProperty("target", target->getControlAddress()); //Need to be global
	}

	return data;
}

void FastMap::loadJSONDataInternal(var data)
{
	if (data.getDynamicObject()->hasProperty("reference"))
	{
		reference->loadJSONData(data.getDynamicObject()->getProperty("reference"));
	}

	if (data.getDynamicObject()->hasProperty("target"))
	{
		String cAddress = data.getDynamicObject()->getProperty("target").toString();
		//Need to be global
		cAddress = cAddress.substring(1);
		setTarget(NodeManager::getInstance()->getControllableForAddress(cAddress));
	}
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
