/*
 ==============================================================================

 MinMaxParameter.h
 Created: 5 Sep 2017 1:57:42pm
 Author:  Martin Hermant

 ==============================================================================
 */

#pragma once

#include "Parameter.h"



class MinMaxParameter : public ParameterBase
{
public:
    MinMaxParameter ( const String& niceName, const String& description,
                      const var& initialValue = 0, const var min = var::undefined(), const var max = var::undefined(),
                     bool enabled = true);
    virtual ~MinMaxParameter();

    var minimumValue;
    var maximumValue;

    bool hasFiniteBounds() const;
    template<class T>
    T getNumericMin()const{
        return minimumValue.isUndefined()?std::numeric_limits<T>::min():(T)minimumValue;
    }
    template<class T>
    T getNumericMax()const{
        return maximumValue.isUndefined()?std::numeric_limits<T>::max():(T)maximumValue;
    }

    DynamicObject* createObject() override;

    void configureFromObject (DynamicObject* ob) override ;

    virtual void setMinMax (var min, var max,ParameterBase::Listener * notifier = nullptr);

    void setUnboundedMax(ParameterBase::Listener * notifier = nullptr);
    void setUnboundedMin(ParameterBase::Listener * notifier = nullptr);
    void setUnbounded(ParameterBase::Listener * notifier = nullptr);
    bool hasFiniteBounds();

    static const Identifier minValueIdentifier;
    static const Identifier maxValueIdentifier;

};
