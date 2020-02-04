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
    ~NumericParameter() {}

    void setValueInternal (const var& _value) override;

    void setNormalizedValue (const floatParamType  normalizedValue, bool silentSet = false, bool force = false);

    floatParamType getNormalizedValue() const;
    bool checkValueIsTheSame (const var& v1, const var& v2) override;
    void setPrecision(int p);
    uint32 precisionMask;
    static T UNBOUNDEDVALUE;
    DECLARE_OBJ_TYPE (NumericParameter,"Numeric parameter")
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (NumericParameter)
};

typedef NumericParameter<int> IntParameter;
typedef NumericParameter<floatParamType> FloatParameter;

#endif  // INTPARAMETER_H_INCLUDED
