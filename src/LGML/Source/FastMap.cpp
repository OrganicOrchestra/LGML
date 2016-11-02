/*
  ==============================================================================

    FastMap.cpp
    Created: 17 May 2016 6:05:27pm
    Author:  bkupe

  ==============================================================================
*/

#include "FastMap.h"
#include "NodeManager.h"
#include "Engine.h"

extern Engine * getEngine();

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
//	setGhostAddress(String::empty);

	fastMapListeners.call(&FastMapListener::fastMapRemoved, this);
}

void FastMap::process()
{
	if (!enabledParam->boolValue()) return;

	float sourceVal = (float)reference->getValue();
	if (target == nullptr) return;

	bool newIsInRange = (sourceVal > minInputVal->floatValue() && sourceVal <= maxInputVal->floatValue());

	if (invertParam->boolValue()) newIsInRange = !newIsInRange;

	if (target->type == Controllable::TRIGGER)
	{
		if (newIsInRange != isInRange && newIsInRange) ((Trigger*)target)->trigger();
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
		
		float normMin = minInputVal->getNormalizedValue();
		float normMax = maxInputVal->getNormalizedValue();
		minInputVal->setRange(reference->currentVariable->parameter->minimumValue, reference->currentVariable->parameter->maximumValue);
		maxInputVal->setRange(reference->currentVariable->parameter->minimumValue, reference->currentVariable->parameter->maximumValue);
		
		minInputVal->setNormalizedValue(normMin);
		maxInputVal->setNormalizedValue(normMax);
		
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

void FastMap::setGhostAddress(const String & address)
{
	if (ghostAddress == address) return;
	ghostAddress = address;
	if (ghostAddress.isNotEmpty()) getEngine()->addControllableContainerListener(this);
	else getEngine()->removeControllableContainerListener(this);
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
		setGhostAddress(String::empty);
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
		Controllable * c = NodeManager::getInstance()->getControllableForAddress(cAddress);
		if (c == nullptr) setGhostAddress(cAddress);
		else setTarget(c);
	}
}

void FastMap::remove()
{
	fastMapListeners.call(&FastMapListener::askForRemoveFastMap, this);
}

void FastMap::childStructureChanged(ControllableContainer *, ControllableContainer *)
{
	
	if (ghostAddress.isNotEmpty() && target == nullptr)
	{
		if (NodeManager::getInstanceWithoutCreating() == nullptr) return;
		Controllable * c = NodeManager::getInstance()->getControllableForAddress(ghostAddress);
		if (c != nullptr) setTarget(c);
	}
}

void FastMap::referenceValueChanged(ControlVariableReference *)
{
	process();
}

void FastMap::referenceVariableChanged(ControlVariableReference *)
{
	float normMin = minInputVal->getNormalizedValue();
	float normMax = maxInputVal->getNormalizedValue();

	if (reference->currentVariable != nullptr)
	{
		minInputVal->setRange(reference->currentVariable->parameter->minimumValue, reference->currentVariable->parameter->maximumValue);
		maxInputVal->setRange(reference->currentVariable->parameter->minimumValue, reference->currentVariable->parameter->maximumValue);
		minInputVal->setNormalizedValue(normMin);
		maxInputVal->setNormalizedValue(normMax);
	}

}

void FastMap::controllableRemoved(Controllable * c)
{
	if (c == target)
	{
		setTarget(nullptr);
	}
}
