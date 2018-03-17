/*
 ==============================================================================

 MinMaxParameter.h
 Created: 5 Sep 2017 1:57:42pm
 Author:  Martin Hermant

 ==============================================================================
 */

#pragma once

#include "Parameter.h"

namespace
{
static const Identifier minValueIdentifier ("_min");
static const Identifier maxValueIdentifier ("_max");
}
class MinMaxParameter : public ParameterBase
{
public:
    MinMaxParameter ( const String& niceName, const String& description,
                      const var& initialValue = 0, const var min = var::undefined(), const var max = var::undefined(),
                      bool enabled = true):
       ParameterBase (niceName, description, initialValue, enabled),
        minimumValue (min),
        maximumValue (max)
    {

    }
    virtual ~MinMaxParameter() {};

    var minimumValue;
    var maximumValue;


    DynamicObject* getObject() override
    {
        auto res = ParameterBase::getObject();
        res->setProperty (minValueIdentifier, minimumValue);
        res->setProperty (maxValueIdentifier, maximumValue);
        return res;
    }

    void configureFromObject (DynamicObject* ob) override
    {
        ParameterBase::configureFromObject (ob);

        if (ob)
        {
            if (ob->hasProperty (minValueIdentifier)) {minimumValue = ob->getProperty (minValueIdentifier);}

            if (ob->hasProperty (maxValueIdentifier)) {maximumValue = ob->getProperty (maxValueIdentifier);}
        }
        else
        {
            jassertfalse;
        }
    }

    void setMinMax (var min, var max)
    {
        minimumValue = min;
        maximumValue = max;
        //  check validity of parameter
        setValue(value,false,true);
        listeners.call (&Listener::parameterRangeChanged, this);
        var arr;
        arr.append (minimumValue);
        arr.append (maximumValue);
        queuedNotifier.addMessage (new ParamWithValue (this, arr, true));
    }

};
