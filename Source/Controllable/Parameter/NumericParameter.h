/* Copyright © Organic Orchestra, 2017
*
* This file is part of LGML.  LGML is a software to manipulate sound in realtime
*
* This program is free software; you can redistribute it and/or modify
* it under the terms of the GNU General Public License as published by
* the Free Software Foundation (version 3 of the License).
*
* This program is distributed in the hope that it will be useful,
* but WITHOUT ANY WARRANTY; without even the implied warranty of
* MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
*
*/


#ifndef INTPARAMETER_H_INCLUDED
#define INTPARAMETER_H_INCLUDED

#include "MinMaxParameter.h"



template<typename T>
class NumericParameter : public MinMaxParameter
{
public:
    NumericParameter (const String& niceName, const String& description = "", const T initialValue = 0, const T minimumValue = 0, const T maximumValue = 1, bool enabled = true);
    ~NumericParameter() {};

    void setValueInternal (const var& _value) override;

    void setNormalizedValue (const float  normalizedValue, bool silentSet = false, bool force = false);

    float getNormalizedValue() const;

    DECLARE_OBJ_TYPE (NumericParameter,"Numeric parameter")
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (NumericParameter)
};

typedef NumericParameter<int> IntParameter;
typedef NumericParameter<double> FloatParameter;

// template implementation

template<class T>
float NumericParameter<T>::getNormalizedValue() const
{
    if ((T)minimumValue == (T)maximumValue)
    {
        return 0.0f;
    }
    else
        return (float)(jmap<float> ((float)value, (float)minimumValue, (float)maximumValue, 0.f, 1.f));
}

template<class T>
void NumericParameter<T>::setNormalizedValue (const float  normalizedValue, bool silentSet, bool force)
{
    setValue ((T)(jmap<float> (normalizedValue, (float)minimumValue, (float)maximumValue)), silentSet, force);
}

#endif  // INTPARAMETER_H_INCLUDED
