/*
 ==============================================================================

 Copyright © Organic Orchestra, 2017

 This file is part of LGML. LGML is a software to manipulate sound in realtime

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

#include "../Engine.h"


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
    fullSync = addNewParameter<BoolParameter> ("FullSync", "synchronize source parameter too", true);
}

FastMap::~FastMap()
{
    referenceOut->removeParameterProxyListener (this);
    referenceIn->removeParameterProxyListener (this);

}
void FastMap::onContainerParameterChanged (Parameter* p)
{
    if (p == invertParam || p == inputRange || p == outputRange || p == fullSync)
    {
        if (referenceIn->get() && referenceOut->get())
        {
            process();
        }
    }
}
void FastMap::process (bool toReferenceOut)
{
    if (!enabledParam->boolValue()) return;

    if (!referenceIn->get() || !referenceOut->get()) return;

    if (fastMapIsProcessing) return;


    auto inRef = toReferenceOut ? referenceIn->get() : referenceOut->get();
    auto sourceVal = (float)inRef->floatValue();
    auto inRange = (toReferenceOut ? inputRange : outputRange);

    float minIn = inRange->getRangeMin();
    float maxIn = inRange->getRangeMax();
    bool newIsInRange = (sourceVal > minIn && sourceVal <= maxIn);

    if (invertParam->boolValue()) newIsInRange = !newIsInRange;

    auto outRef = (toReferenceOut ? referenceOut : referenceIn)->get();

    while (auto* prox = dynamic_cast<ParameterProxy*> (outRef))
    {
        outRef = prox->linkedParam;
    }

    if (!outRef) return;

    fastMapIsProcessing = true;
    auto type = outRef->getTypeId();

    if (type == Trigger::_objType)
    {
        if ((newIsInRange != isInRange && newIsInRange) || inRef->getTypeId() == Trigger::_objType) ((Trigger*)outRef)->trigger();
    }
    else
    {
        if (type == BoolParameter::_objType)
        {
            if (inRef->getTypeId() == Trigger::_objType)
            {
                ((BoolParameter*)outRef)->setValue (!outRef->boolValue());
            }
            else
            {
                ((BoolParameter*)outRef)->setValue (newIsInRange);
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

                ((Parameter*)outRef)->setValue (targetVal);
            }
        }
    }

    isInRange = newIsInRange;
    fastMapIsProcessing = false;

}






void FastMap::linkedParamValueChanged (ParameterProxy* p)
{
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
void FastMap::linkedParamChanged (ParameterProxy* p)
{

    if (p == referenceIn )
    {
        if (p->linkedParam == referenceOut->linkedParam)
        {
            if (p->linkedParam)
            {

                LOG ("!!Can't map a parameter to itself");
                // ignore assert for loopBacks
                referenceIn->isSettingValue = false;
                referenceIn->setParamToReferTo (nullptr);
            }
        }
        else
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
            inputRange->setValue (newMin, newMax);



        }

    }
    else if (p == referenceOut)
    {
        if (p->linkedParam == referenceIn->linkedParam)
        {
            if (p->linkedParam)
            {
                LOG ("!!Can't map a parameter to itself");
                // ignore assert for loopBacks
                referenceOut->isSettingValue = false;
                referenceOut->setParamToReferTo (nullptr);
            }
        }
        else if (p->linkedParam && !p->linkedParam->isEditable)
        {
            LOG ("!!Parameter non editable");
            // ignore assert for loopBacks
            referenceOut->isSettingValue = false;
            referenceOut->setParamToReferTo (nullptr);
        }
        else
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
            outputRange->setValue (newMin, newMax);


        }

    }


};
