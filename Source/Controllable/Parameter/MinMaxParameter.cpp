/*
  ==============================================================================

    MinMaxParameter.cpp
    Created: 3 Feb 2020 3:53:27pm
    Author:  Martin Hermant

  ==============================================================================
*/

#include "MinMaxParameter.h"

const Identifier MinMaxParameter::minValueIdentifier ("_min");
const Identifier MinMaxParameter::maxValueIdentifier ("_max");


MinMaxParameter::MinMaxParameter ( const String& niceName, const String& description,
                 const var& initialValue, const var min , const var max,
                 bool enabled):
ParameterBase (niceName, description, initialValue, enabled),
minimumValue (min),
maximumValue (max)
{

}
MinMaxParameter::~MinMaxParameter() {}


bool MinMaxParameter::hasFiniteBounds() const{
    return !minimumValue.isUndefined() && !maximumValue.isUndefined();
}


DynamicObject* MinMaxParameter::createObject()
{
    auto res = ParameterBase::createObject();
    res->setProperty (minValueIdentifier, minimumValue);
    res->setProperty (maxValueIdentifier, maximumValue);
    return res;
}

void MinMaxParameter::configureFromObject (DynamicObject* ob)
{


    if (ob)
    {
        if (ob->hasProperty (minValueIdentifier)) {minimumValue = ob->getProperty (minValueIdentifier);}

        if (ob->hasProperty (maxValueIdentifier)) {maximumValue = ob->getProperty (maxValueIdentifier);}
    }
    else
    {
        jassertfalse;
    }

    ParameterBase::configureFromObject (ob);
}

void MinMaxParameter::setMinMax (var min, var max,ParameterBase::Listener * notifier )
{
    minimumValue = min;
    maximumValue = max;
    //  check validity of parameter
    setValueFrom(notifier,value,false,false);
    listeners.call (&Listener::parameterRangeChanged, this);
    var arr;
    arr.append (minimumValue);
    arr.append (maximumValue);
    queuedNotifier->addMessage (new ParamWithValue (this, arr, true));
}

void MinMaxParameter::setUnboundedMax(ParameterBase::Listener * notifier){
    setMinMax(minimumValue, var::undefined(),notifier);
}
void MinMaxParameter::setUnboundedMin(ParameterBase::Listener * notifier){
    setMinMax(var::undefined(), maximumValue,notifier);
}
void MinMaxParameter::setUnbounded(ParameterBase::Listener * notifier){
    setMinMax(var::undefined(), var::undefined(),notifier);
}
bool MinMaxParameter::hasFiniteBounds(){
    return !minimumValue.isUndefined() && !maximumValue.isUndefined();
}
