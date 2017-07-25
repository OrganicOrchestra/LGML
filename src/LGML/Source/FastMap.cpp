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


FastMap::FastMap() :
	referenceIn(nullptr),
	referenceOut(nullptr),
	ControllableContainer("FastMap")
{
//	reference = new ControlVariableReference();
//	referenceIn.get()->addReferenceListener(this);

  referenceIn = addNewParameter<ParameterProxy>("in param","parameter for input");
  referenceOut = addNewParameter<ParameterProxy>("out param","parameter for input");
	enabledParam = addBoolParameter("Enabled", "Enabled / Disable Fast Map", true);

	minInputVal = addFloatParameter("In Min", "Minimum Input Value", 0, 0, 1);
	maxInputVal = addFloatParameter("In Max", "Maximum Input Value", 1, 0, 1);
	minOutputVal = addFloatParameter("Out Min", "Minimum Output Value", 0, 0, 1);
	maxOutputVal = addFloatParameter("Out Max", "Maximum Output Value", 1, 0, 1);

	invertParam = addBoolParameter("Invert", "Invert the output signal", false);

}

FastMap::~FastMap()
{


	fastMapListeners.call(&FastMapListener::fastMapRemoved, this);
	if(getEngine())  getEngine()->removeControllableContainerListener(this);
}

void FastMap::process()
{
	if (!enabledParam->boolValue()) return;
  if(!referenceIn->get() || !referenceOut->get()) return;

  auto inRef = referenceIn->get();
  if(inRef){
	float sourceVal = (float)inRef->value;

	bool newIsInRange = (sourceVal > minInputVal->floatValue() && sourceVal <= maxInputVal->floatValue());

	if (invertParam->boolValue()) newIsInRange = !newIsInRange;

    auto outRef = referenceOut->get();
	if (outRef->type == Controllable::TRIGGER)
	{
		if (newIsInRange != isInRange && newIsInRange) ((Trigger*)outRef)->trigger();
	}
	else
	{
		if (outRef->type == Controllable::BOOL)
		{
			((BoolParameter *)outRef)->setValue(newIsInRange);
		}else
		{
			if (minOutputVal->floatValue() < maxOutputVal->floatValue())
			{
				float targetVal = juce::jmap<float>(sourceVal, minInputVal->floatValue(), maxInputVal->floatValue(), minOutputVal->floatValue(), maxOutputVal->floatValue());
				targetVal = juce::jlimit<float>(minOutputVal->floatValue(), maxOutputVal->floatValue(), targetVal);
				if (invertParam->boolValue()) targetVal = maxOutputVal->floatValue() - (targetVal - minOutputVal->floatValue());
				((Parameter *)outRef)->setNormalizedValue(targetVal);
			}
		}
	}

	isInRange = newIsInRange;
  }
}




void FastMap::setGhostAddress(const String & address)
{
	if (ghostAddress == address) return;
	ghostAddress = address;
	if (ghostAddress.isNotEmpty()) getEngine()->addControllableContainerListener(this);
	else getEngine()->removeControllableContainerListener(this);
}


void FastMap::remove()
{
	fastMapListeners.call(&FastMapListener::askForRemoveFastMap, this);
}



void FastMap::linkedParamValueChanged(ParameterProxy *p) {
  if(p==referenceIn){
    process();
    return;
  }
};
void FastMap::linkedParamChanged(ParameterProxy *p) {

  if (p==referenceIn)
  {

    float normMin = minInputVal->getNormalizedValue();
    float normMax = maxInputVal->getNormalizedValue();
    //		minInputVal->setRange(referenceIn.get()->currentVariable->parameter->minimumValue, referenceIn.get()->currentVariable->parameter->maximumValue);
    //		maxInputVal->setRange(referenceIn.get()->currentVariable->parameter->minimumValue, referenceIn.get()->currentVariable->parameter->maximumValue);
    //
    minInputVal->setNormalizedValue(normMin);
    maxInputVal->setNormalizedValue(normMax);
    fastMapListeners.call(&FastMapListener::fastMapReferenceChanged, this);
  }
  else if(p==referenceOut){
    fastMapListeners.call(&FastMapListener::fastMapTargetChanged, this);
  }

  
};
