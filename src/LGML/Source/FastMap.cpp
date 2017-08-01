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

  inputRange = addNewParameter<RangeParameter>("In Range", "Input Range", 0.0f,1.0f,0.0f,1.0f);
  outputRange = addNewParameter<RangeParameter>("Out Range", "Out Range", 0.0f,1.0f, 0.0f, 1.0f);

  invertParam = addNewParameter<BoolParameter>("Invert", "Invert the output signal", false);
  fullSync = addNewParameter<BoolParameter>("FullSync", "synchronize source parameter too", true);
}

FastMap::~FastMap()
{
  referenceOut->removeParameterProxyListener(this);
  referenceIn->removeParameterProxyListener(this);

}
void FastMap::onContainerParameterChanged(Parameter *p){
  if(p==invertParam || p==inputRange || p==outputRange || p==fullSync){
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
  auto inRange = (toReferenceOut?inputRange:outputRange);

  float minIn = inRange->getRangeMin();
  float maxIn = inRange->getRangeMax();
  bool newIsInRange = (sourceVal > minIn && sourceVal <= maxIn);

  if (invertParam->boolValue()) newIsInRange = !newIsInRange;

  auto outRef = (toReferenceOut?referenceOut:referenceIn)->get();
  fastMapIsProcessing = true;
  if (outRef->type == Controllable::TRIGGER)
  {
    if ((newIsInRange != isInRange && newIsInRange) || inRef->type==Controllable::TRIGGER) ((Trigger*)outRef)->trigger();
  }
  else
  {
    if (outRef->type == Controllable::BOOL){
      if(inRef->type== Controllable::TRIGGER){
        ((BoolParameter *)outRef)->setValue(!outRef->boolValue());
      }
      else{
        ((BoolParameter *)outRef)->setValue(newIsInRange);
      }
    }else
    {
      if ( minIn != maxIn)
      {
        auto outRange = (toReferenceOut?outputRange:inputRange);
        float minOut = outRange->getRangeMin();
        float maxOut = outRange->getRangeMax();
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

      float newMin = referenceIn->linkedParam?(float)referenceIn->linkedParam->minimumValue:0;
      float newMax = referenceIn->linkedParam?(float)referenceIn->linkedParam->maximumValue:1;
      inputRange->setRange(newMin,newMax);




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

      float newMin = referenceOut->linkedParam?(float)referenceOut->linkedParam->minimumValue:0;
      float newMax = referenceOut->linkedParam?(float)referenceOut->linkedParam->maximumValue:1;
      outputRange->setRange(newMin,newMax);
      
      
    }
    
  }
  
  
};
