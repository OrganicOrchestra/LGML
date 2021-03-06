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
extern bool isEngineLoadingFile();

struct Smoother : private Timer{

    typedef  std::function<void(float,float)> CBTYPE;
    CBTYPE cb;
    Smoother(int _granularity=10):
    pct(0),
    granularity(_granularity),
    state(IDLE){
        lastTime = Time::currentTimeMillis();
    }
    ~Smoother(){
        stopTimer();
    }
    void rampUp(float timeIn,CBTYPE _cb,float _maxValue ){ //  TODO do we need immediate? if yes need to check that timer was started before stopping timer
        if(state==IN){
            jassertfalse; // do we retrigger?
        }
        maxValue = _maxValue;
        stopTimer();
        rampTime =0;
        if(state==OUT){
            rampTime = pct*rampTotalTime;
        }
        rampTotalTime = jmax(1.0f,timeIn);

        state = IN;
        lastTime = Time::currentTimeMillis()-1;
        cb = _cb;
        startTimer(granularity);
    }

    void rampDown(float timeOut,CBTYPE _cb ){
        if(state==OUT){
            jassertfalse; // do we retrigger?
        }

        stopTimer();
        rampTime = 0;
        rampTotalTime = jmax(1.0f,timeOut);
        if(state==IN){
            rampTime = (1.0f-pct)*rampTotalTime;
        }
        state = OUT;
        lastTime = Time::currentTimeMillis()-1;
        cb = _cb;
        startTimer(granularity);
    }
    bool isRamping(){
        return state!=IDLE;
    }

    void timerCallback()final{
        auto ct = Time::currentTimeMillis();
        auto delta = ct-lastTime;
        rampTime+=delta;
        pct = jmin(1.0f,rampTime/rampTotalTime);
        if(state==OUT){pct = 1.0f - pct;}
        cb(pct,maxValue);
        if(pct==1.0 || pct==0.0){
            state=IDLE;
            stopTimer();
        }
        lastTime = ct;

    }

    void reset(){
        stopTimer();
        state=IDLE;
    }

    typedef enum{
        IN,
        OUT,
        IDLE
    }State;
    State state;

    float pct;
    long long lastTime;
    float rampTime,rampTotalTime;
    int granularity;
    float maxValue;
};


FastMap::FastMap() :
referenceIn (nullptr),
referenceOut (nullptr),
fastMapIsProcessing (false),
smoother(std::make_unique<Smoother>()),
ParameterContainer ("FastMap")
{

    referenceIn = addNewParameter<ParameterProxy> ("in param", "parameter for input");
    referenceIn->addParameterProxyListener (this);
    referenceOut = addNewParameter<ParameterProxy> ("out param", "parameter for input");
    referenceOut->addParameterProxyListener (this);
    enabledParam = addNewParameter<BoolParameter> ("Enabled", "Enabled / Disable Fast Map", true);

    inputRange = addNewParameter<RangeParameter> ("In Range", "Input Range", 0.0f, 1.0f, 0.0f, 1.0f);
    inputRange->setUnbounded();
    outputRange = addNewParameter<RangeParameter> ("Out Range", "Out Range", 0.0f, 1.0f, 0.0f, 1.0f);
    outputRange->setUnbounded();
    invertParam = addNewParameter<BoolParameter> ("Invert", "Invert the output signal", false);
    toggleParam = addNewParameter<BoolParameter> ("Toggle", "Toggles the output signal", false);
    fullSync = addNewParameter<BoolParameter> ("FullSync", "synchronize source parameter too", true);
    smoothTimeIn = addNewParameter<FloatParameter> ("smoothTimeIn", "time in ms to go to non-zero", 0.0f,0.0f,10.0f);
    smoothTimeOut = addNewParameter<FloatParameter> ("smoothTimeOut", "time in ms to go to zero", 0.0f,0.0f,10.0f);
}

FastMap::~FastMap()
{
    masterReference.clear();
    referenceOut->removeParameterProxyListener (this);
    referenceIn->removeParameterProxyListener (this);

}
void FastMap::onContainerParameterChanged ( ParameterBase* p)
{
    if ((p==smoothTimeIn || p==smoothTimeOut) ){
        if(smoothingEnabled() && fullSync->boolValue()){
        fullSync->setValueFrom(this,false);
        fullSync->setEnabled(false);
        }
        else if(!smoothingEnabled() && !fullSync->enabled){
            fullSync->setValueFrom(this,true);
            fullSync->setEnabled(true);
        }
    }
    if (p == invertParam || p == inputRange || p == outputRange || p == fullSync)
    {
        if(p==fullSync){
            smoothTimeIn->setEnabled(!fullSync->boolValue());
            smoothTimeOut->setEnabled(!fullSync->boolValue());
        }
        if (referenceIn->getLinkedParam() && referenceOut->getLinkedParam())
        {
            process(true,false);
        }
    }
}
void FastMap::process (bool toReferenceOut,bool sourceHasChanged)
{
    if (!enabledParam->boolValue()) return;

    if (!referenceIn->getLinkedParam() || !referenceOut->getLinkedParam()) return;

    if (fastMapIsProcessing) return;

    if( isEngineLoadingFile()){
        return ;
    }


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
        
        if (inRef->getFactoryTypeId() == Trigger::_factoryType ||
            (newIsInRange != isInRange && (newIsInRange || toggleParam->boolValue()))
             ) {
            ((Trigger*)outRef)->triggerFrom(this);
        }
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
                else{
                    float targetToggleValue =outRef->boolValue()?maxIn-10e-6:minIn;// to force re update?
                    MessageManager::callAsync([=](){
                        (inRef)->setValueFrom (this,targetToggleValue,false,false);
                    });
                }

            }
            else
            {
                ((BoolParameter*)outRef)->setValueFrom (this,newIsInRange);
            }
        }else if((type == EnumParameter::_factoryType) || (type == StringParameter::_factoryType)){
            outRef->setValueFrom(this, inRef->stringValue());
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

                if(smoothingEnabled()){
//                    bool isToggleVal=sourceVal==minIn || sourceVal == maxIn
                    WeakReference<ParameterBase> wkp = (( ParameterBase*)outRef);
                    auto cb = [=](float pct,float maxV) mutable{
                        if(wkp){
                            float inScale = maxV/(maxIn-minIn);
                            float smoothVal = minOut+pct*(maxOut-minOut)*inScale;
                            if (invertParam->boolValue()) smoothVal = maxOut - (smoothVal - minOut);
                            wkp->setValueFrom(this, smoothVal);
                        }
                    };

                    if(sourceHasChanged){
                        bool sourceToggleState = sourceVal>minIn;
                        if(sourceToggleState){
                            smoother->rampUp((int)(smoothTimeIn->floatValue()*1000.0),cb,sourceVal);
                        }
                        else{
                            smoother->rampDown((int)(smoothTimeOut->floatValue()*1000.0),cb);
                        }
                        
                    }
                }
                else{

                (( ParameterBase*)outRef)->setValueFrom (this,targetVal);
                }
            }
        }
    }

    isInRange = newIsInRange;
    fastMapIsProcessing = false;

}

bool FastMap::smoothingEnabled(){
    return smoothTimeOut->floatValue()>0 || smoothTimeIn->floatValue()>0;
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
        var newMin = mmp ? mmp->minimumValue : var::undefined();
        var newMax = mmp ? mmp->maximumValue : var::undefined();
        inputRange->setMinMax (newMin, newMax);
        if(inputRange->hasFiniteBounds()){
            if(inputRange->getRangeMin() < (float)newMin || inputRange->getRangeMax()>(float)newMax){ // modify only if range changed a lot
                inputRange->setValue (newMin,newMax);
            }
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
        var newMin = mmp ? mmp->minimumValue : var::undefined();
        var newMax = mmp ? mmp->maximumValue : var::undefined();
        outputRange->setMinMax (newMin, newMax);
        if(outputRange->hasFiniteBounds()){
            if(outputRange->getRangeMin() < (float)newMin || outputRange->getRangeMax()>(float)newMax){ // modify only if range changed a lot
                outputRange->setValue (newMin,newMax);
            }

        }



        
    }


    
};
