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
fastMapIsProcessing(false),
ControllableContainer("FastMap")
{

  referenceIn = addNewParameter<ParameterProxy>("in param","parameter for input");
  referenceIn->addParameterProxyListener(this);
  referenceOut = addNewParameter<ParameterProxy>("out param","parameter for input");
  referenceOut->addParameterProxyListener(this);
  enabledParam = addNewParameter<BoolParameter>("Enabled", "Enabled / Disable Fast Map", true);

  minInputVal = addNewParameter<FloatParameter>("In Min", "Minimum Input Value", 0, 0, 1);
  maxInputVal = addNewParameter<FloatParameter>("In Max", "Maximum Input Value", 1, 0, 1);
  minOutputVal = addNewParameter<FloatParameter>("Out Min", "Minimum Output Value", 0, 0, 1);
  maxOutputVal = addNewParameter<FloatParameter>("Out Max", "Maximum Output Value", 1, 0, 1);

  invertParam = addNewParameter<BoolParameter>("Invert", "Invert the output signal", false);
  fullSync = addNewParameter<BoolParameter>("FullSync", "synchronize source parameter too", false);
}

FastMap::~FastMap()
{

}
void FastMap::onContainerParameterChanged(Parameter *p){
  if(p==invertParam || p==minInputVal || p==maxInputVal || p==minOutputVal || p== maxOutputVal || p==fullSync){
    if(referenceIn->get() && referenceOut->get()){
      process();
    }
  }
}
void FastMap::process(bool toReferenceOut)
{
  if (!enabledParam->boolValue()) return;
  if(!referenceIn->get() || !referenceOut->get()) return;
  if(fastMapIsProcessing) return;


  auto inRef = toReferenceOut?referenceIn->get():referenceOut->get();
  auto sourceVal = (float)inRef->floatValue();
  float minIn = (toReferenceOut?minInputVal:minOutputVal)->floatValue();
  float maxIn = (toReferenceOut?maxInputVal:maxOutputVal)->floatValue();
  bool newIsInRange = (sourceVal > minIn && sourceVal <= maxIn);

  if (invertParam->boolValue()) newIsInRange = !newIsInRange;

  auto outRef = (toReferenceOut?referenceOut:referenceIn)->get();
  fastMapIsProcessing = true;
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
      if ( minIn != maxIn)
      {
        float minOut = (toReferenceOut?minOutputVal:minInputVal)->floatValue();
        float maxOut = (toReferenceOut?maxOutputVal:maxInputVal)->floatValue();
        float targetVal = juce::jmap<float>(sourceVal, minIn, maxIn, minOut,maxOut);
        targetVal = juce::jlimit<float>(minOut,maxOut, targetVal);
        if (invertParam->boolValue()) targetVal = maxOut - (targetVal - minOut);
        ((Parameter *)outRef)->setValue(targetVal);
      }
    }
  }

  isInRange = newIsInRange;
  fastMapIsProcessing=false;

}






void FastMap::linkedParamValueChanged(ParameterProxy *p) {
  if(p==referenceIn){
    process();
    return;
  }
  else if(p==referenceOut && fullSync->boolValue()){
    process(false);
    return;
  }
};
void FastMap::linkedParamChanged(ParameterProxy *p) {

  if (p==referenceIn )
  {
    if(p->linkedParam==referenceOut->linkedParam){
      LOG("Can't map a parameter to itself");
      // ignore assert for loopBacks
      referenceIn->isSettingValue=false;
      referenceIn->setParamToReferTo(nullptr);
    }
    else{
      float normMin = minInputVal->getNormalizedValue();
      float normMax = maxInputVal->getNormalizedValue();
      float newMin = referenceIn->linkedParam?(float)referenceIn->linkedParam->minimumValue:0;
      float newMax = referenceIn->linkedParam?(float)referenceIn->linkedParam->maximumValue:1;
      minInputVal->setRange(newMin,newMax);
      maxInputVal->setRange(newMin,newMax);

      minInputVal->setNormalizedValue(normMin);
      maxInputVal->setNormalizedValue(normMax);

    }

  }
  else if(p==referenceOut){
    if(p->linkedParam==referenceIn->linkedParam){
      LOG("Can't map a parameter to itself");
      // ignore assert for loopBacks
      referenceOut->isSettingValue=false;
      referenceOut->setParamToReferTo(nullptr);
    }
    else if(p->linkedParam && !p->linkedParam->isEditable){
      LOG("Parameter non editable");
      // ignore assert for loopBacks
      referenceOut->isSettingValue=false;
      referenceOut->setParamToReferTo(nullptr);
    }
    else{
      float normMin = minOutputVal->getNormalizedValue();
      float normMax = maxOutputVal->getNormalizedValue();
      float newMin = referenceOut->linkedParam?(float)referenceOut->linkedParam->minimumValue:0;
      float newMax = referenceOut->linkedParam?(float)referenceOut->linkedParam->maximumValue:1;
      minOutputVal->setRange(newMin,newMax);
      maxOutputVal->setRange(newMin,newMax);

      minOutputVal->setNormalizedValue(normMin);
      maxOutputVal->setNormalizedValue(normMax);

    }

  }


};
