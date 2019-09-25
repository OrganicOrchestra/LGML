/*
 ==============================================================================

 Copyright © Organic Orchestra, 2017

 This file is part of LGML. LGML is a software to manipulate sound in real-time

 This program is free software; you can redistribute it and/or modify
 it under the terms of the GNU General Public License as published by
 the Free Software Foundation (version 3 of the License).

 This program is distributed in the hope that it will be useful,
 but WITHOUT ANY WARRANTY; without even the implied warranty of
 MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.

 ==============================================================================
 */

#include "FastMap.h"
#include "../Node/Manager/NodeManager.h"

//#include "../Engine.h"

FastMap::FastMap() :
referenceIn (nullptr),
referenceOut (nullptr),
fastMapIsProcessing (false),
ParameterContainer ("FastMap")
{

    referenceIn = addNewParameter<ParameterProxy> ("in param", "parameter for input");
    referenceIn->addParameterProxyListener (this);
    referenceOut = addNewParameter<ParameterProxy> ("out param", "parameter for input");
    referenceOut->addParameterProxyListener (this);
    enabledParam = addNewParameter<BoolParameter> ("Enabled", "Enabled / Disable Fast Map", true);

    inputRange = addNewParameter<RangeParameter> ("In Range", "Input Range", 0.0f, 1.0f, 0.0f, 1.0f);
    outputRange = addNewParameter<RangeParameter> ("Out Range", "Out Range", 0.0f, 1.0f, 0.0f, 1.0f);

    invertParam = addNewParameter<BoolParameter> ("Invert", "Invert the output signal", false);
    toggleParam = addNewParameter<BoolParameter> ("Toggle", "Toggles the output signal", false);
    fullSync = addNewParameter<BoolParameter> ("FullSync", "synchronize source parameter too", true);
}

FastMap::~FastMap()
{
    masterReference.clear();
    referenceOut->removeParameterProxyListener (this);
    referenceIn->removeParameterProxyListener (this);

}
void FastMap::onContainerParameterChanged ( ParameterBase* p)
{
    if (p == invertParam || p == inputRange || p == outputRange || p == fullSync)
    {
        if (referenceIn->getLinkedParam() && referenceOut->getLinkedParam())
        {
            process();
        }
    }
}
void FastMap::process (bool toReferenceOut)
{
    if (!enabledParam->boolValue()) return;

    if (!referenceIn->getLinkedParam() || !referenceOut->getLinkedParam()) return;

    if (fastMapIsProcessing) return;


    auto inRef = toReferenceOut ? referenceIn->getLinkedParam() : referenceOut->getLinkedParam();
    auto sourceVal = (float)inRef->floatValue();
    auto inRange = (toReferenceOut ? inputRange : outputRange);

    float minIn = inRange->getRangeMin();
    float maxIn = inRange->getRangeMax();
    bool newIsInRange = (sourceVal > minIn && sourceVal <= maxIn);

    if (invertParam->boolValue()) newIsInRange = !newIsInRange;

    auto outRef = (toReferenceOut ? referenceOut : referenceIn)->getLinkedParam();

    while (auto* prox = dynamic_cast<ParameterProxy*> (outRef))
    {
        outRef = prox->linkedParam;
    }

    if (!outRef) return;

    fastMapIsProcessing = true;
    auto type = outRef->getFactoryTypeId();

    if (type == Trigger::_factoryType)
    {
        if ((newIsInRange != isInRange && newIsInRange) || inRef->getFactoryTypeId() == Trigger::_factoryType) ((Trigger*)outRef)->triggerFrom(this);
    }
    else
    {
        if (type == BoolParameter::_factoryType)
        {
            if ((inRef->getFactoryTypeId() == Trigger::_factoryType ))
            {
                ((BoolParameter*)outRef)->setValueFrom (this,!outRef->boolValue());
            }
            else if(toggleParam->boolValue() ){ // toggles only on max for float values
                if(sourceVal==maxIn){
                    ((BoolParameter*)outRef)->setValueFrom (this,!outRef->boolValue());
                }

            }
            else
            {
                ((BoolParameter*)outRef)->setValueFrom (this,newIsInRange);
            }
        }
        else
        {
            if ( minIn != maxIn)
            {
                auto outRange = (toReferenceOut ? outputRange : inputRange);
                float minOut = outRange->getRangeMin();
                float maxOut = outRange->getRangeMax();
                float targetVal = juce::jmap<float> (sourceVal, minIn, maxIn, minOut, maxOut);
                targetVal = juce::jlimit<float> (minOut, maxOut, targetVal);

                if (invertParam->boolValue()) targetVal = maxOut - (targetVal - minOut);

                (( ParameterBase*)outRef)->setValueFrom (this,targetVal);
            }
        }
    }

    isInRange = newIsInRange;
    fastMapIsProcessing = false;

}






void FastMap::linkedParamValueChanged (ParameterProxy* p,ParameterBase::Listener * notifier)
{
    if(notifier==this){
        return;
    }
    if (p == referenceIn)
    {
        process();
        return;
    }
    else if (p == referenceOut && fullSync->boolValue())
    {
        process (false);
        return;
    }
};

void FastMap::linkedParamRangeChanged(ParameterProxy* p ) {
    float newMin = 0;
    float newMax = 1;
    float newVmin =0;
    float newVmax = 1;
    RangeParameter * rangeToModify(nullptr);
    if(p==referenceIn){
        if(auto mmp = dynamic_cast<MinMaxParameter*> (referenceIn->linkedParam.get())){
            newMin =  (float)mmp->minimumValue ;
            newMax =  (float)mmp->maximumValue ;
        }
        rangeToModify =  inputRange;
    }
    else if (p==referenceOut){
        if(auto mmp = dynamic_cast<MinMaxParameter*> (referenceOut->linkedParam.get())){
            newMin =  (float)mmp->minimumValue ;
            newMax =  (float)mmp->maximumValue ;
        }
        rangeToModify =  outputRange;
    }
    else{
        jassertfalse;
        return;
    }
    bool remapRange =rangeToModify->hasFiniteRange();
    if(remapRange){
        newVmin = rangeToModify->getNormalizedRangeMin();
        newVmax = rangeToModify->getNormalizedRangeMax();
    }
    rangeToModify->setMinMax (newMin, newMax);
    if(remapRange){
        rangeToModify->setNormalizedRangeMinMax(newVmin,newVmax);
    }

};

ParameterProxy * FastMap::getProxyForParameter(ParameterBase* p, bool recursive) const{
    ParameterBase * r = referenceIn?referenceIn->linkedParam:nullptr;
    if(recursive)
        while(auto * prox = dynamic_cast<ParameterProxy*>(r)){r = prox->linkedParam;}
    if(r==p){return referenceIn;}

    r = referenceOut?referenceOut->linkedParam:nullptr;;
    if(recursive)
        while(auto * prox = dynamic_cast<ParameterProxy*>(r)){r = prox->linkedParam;}
    if(r==p){return referenceOut;}

    return nullptr;
}

void FastMap::linkedParamChanged (ParameterProxy* p)
{

    // check forbidden mapping
    if (p && p->linkedParam && (p == referenceIn  || p == referenceOut)){
        bool isForbidden = true;


        if( p->linkedParam == inputRange || p->linkedParam== outputRange){
            LOGW(juce::translate("Can't map a parameter to one of it's own range"));
        }
        else if ( referenceIn->linkedParam && referenceIn->linkedParam == referenceOut->linkedParam)
        {
            LOGW(juce::translate("Can't map a parameter to itself"));
        }
        else if (p==referenceOut && !referenceOut->linkedParam->isEditable)
        {
            LOGW(juce::translate("Parameter non editable"));
        }
        else{
            isForbidden = false;
        }

        if(isForbidden){
            // ignore assert for loopBacks
            //                p->isSettingValue = false;
            WeakReference<ParameterProxy> ref ( p==referenceOut?referenceOut:referenceIn );
            MessageManager::callAsync([ref]()mutable{
                if(ref.get())
                    ref->setParamToReferTo (nullptr);
            });
            return;
        }
    }
    if (p == referenceIn )
    {

        auto* lpar = referenceIn->linkedParam.get();

        while (auto* prox = dynamic_cast<ParameterProxy*> (lpar))
        {
            lpar = prox->linkedParam;
        }

        auto mmp = dynamic_cast<MinMaxParameter*> (lpar);
        float newMin = mmp ? (float)mmp->minimumValue : 0;
        float newMax = mmp ? (float)mmp->maximumValue : 1;
        inputRange->setMinMax (newMin, newMax);
        if(inputRange->getRangeMin() < newMin || inputRange->getRangeMax()>newMax){ // modify only if range changed a lot
            inputRange->setValue (newMin,newMax);
        }
    }
    else if (p == referenceOut)
    {

        auto* lpar = referenceOut->linkedParam.get();

        while (auto* prox = dynamic_cast<ParameterProxy*> (lpar))
        {
            lpar = prox->linkedParam;
        }

        auto mmp = dynamic_cast<MinMaxParameter*> (lpar);
        float newMin = mmp ? (float)mmp->minimumValue : 0;
        float newMax = mmp ? (float)mmp->maximumValue : 1;
        outputRange->setMinMax (newMin, newMax);
        if(outputRange->getRangeMin() < newMin || outputRange->getRangeMax()>newMax){ // modify only if range changed a lot
            outputRange->setValue (newMin,newMax);
        }



        
    }


    
};
