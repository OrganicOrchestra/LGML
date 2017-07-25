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

  referenceIn = addNewParameter<ParameterProxy>("in param","parameter for input");
  referenceIn->addParameterProxyListener(this);
  referenceOut = addNewParameter<ParameterProxy>("out param","parameter for input");
  referenceOut->addParameterProxyListener(this);
	enabledParam = addBoolParameter("Enabled", "Enabled / Disable Fast Map", true);

	minInputVal = addFloatParameter("In Min", "Minimum Input Value", 0, 0, 1);
	maxInputVal = addFloatParameter("In Max", "Maximum Input Value", 1, 0, 1);
	minOutputVal = addFloatParameter("Out Min", "Minimum Output Value", 0, 0, 1);
	maxOutputVal = addFloatParameter("Out Max", "Maximum Output Value", 1, 0, 1);

	invertParam = addBoolParameter("Invert", "Invert the output signal", false);

}

FastMap::~FastMap()
{

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






void FastMap::linkedParamValueChanged(ParameterProxy *p) {
  if(p==referenceIn){
    process();
    return;
  }
};
void FastMap::linkedParamChanged(ParameterProxy *p) {

  if (p==referenceIn )
  {
    if(p==referenceOut){
      LOG("Can't map a parameter to itself");
      referenceIn->setParamToReferTo(nullptr);
    }
    else{
    float normMin = minInputVal->getNormalizedValue();
    float normMax = maxInputVal->getNormalizedValue();
    //		minInputVal->setRange(referenceIn.get()->currentVariable->parameter->minimumValue, referenceIn.get()->currentVariable->parameter->maximumValue);
    //		maxInputVal->setRange(referenceIn.get()->currentVariable->parameter->minimumValue, referenceIn.get()->currentVariable->parameter->maximumValue);
    //
    minInputVal->setNormalizedValue(normMin);
    maxInputVal->setNormalizedValue(normMax);

    }

  }
  else if(p==referenceOut){
    if(p==referenceIn){
      LOG("Can't map a parameter to itself");
      referenceOut->setParamToReferTo(nullptr);
    }
    
  }

  
};
