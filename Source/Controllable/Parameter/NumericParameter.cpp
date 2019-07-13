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


#include "NumericParameter.h"

#include "ParameterFactory.h"
REGISTER_PARAM_TYPE_TEMPLATED (NumericParameter, int,"intParameter")
REGISTER_PARAM_TYPE_TEMPLATED (NumericParameter, floatParamType,"doubleParameter")



template<class T>
T NumericParameter<T>::UNBOUNDEDVALUE = std::numeric_limits<T>::max();

template<class T>
NumericParameter<T>::NumericParameter (const String& niceName, const String& description,
                                       const T  initialValue, const T  minValue, const T  maxValue,
                                       bool enabled) :
MinMaxParameter ( niceName, description, var (initialValue), var (minValue), var (maxValue), enabled)
{
    if (std::is_same<T, int>::value)
    {
        jassert (value.isInt());
        precisionMask=0;
    }
    else if (std::is_same<T, floatParamType>::value)
    {
        jassert (value.isDouble());
        setPrecision(-1);
    }
    else
    {
        // another Numeric parameter type?
        jassertfalse;
    }
    if(minValue==UNBOUNDEDVALUE){setUnboundedMin();}
    if(maxValue==UNBOUNDEDVALUE){setUnboundedMax();}

}

template<class T>
void  NumericParameter<T>::setValueInternal (const var& _value)
{   if(!minimumValue.isUndefined() && !maximumValue.isUndefined()){
    value = jlimit<T> ((T)minimumValue, (T) maximumValue, (T)_value);
}
else if(!minimumValue.isUndefined() && maximumValue.isUndefined()){
    value = jmax<T>((T)minimumValue,(T)_value);
}
else if(minimumValue.isUndefined() && !maximumValue.isUndefined()){
    value = jmin<T>((T)maximumValue,(T)_value);
}
else{
    value = (T)_value;
}
    if(precisionMask>0){
        value = floor((T)_value * precisionMask)/precisionMask;
    }
}


template<class T>
bool  NumericParameter<T>::checkValueIsTheSame (const var& v1, const var& v2){
    if(v1.hasSameTypeAs(v2)){
    T a {v1};
    T b {v2};
    return precisionMask>0?floor(a*precisionMask)==floor(b*precisionMask):a==b;
    }
    return false;
}

template<>
void  NumericParameter<int>::setPrecision(int p){
    jassertfalse; // can't set precision for ints
}

template<class T>
void  NumericParameter<T>::setPrecision(int p){
    if(p<0){
        precisionMask = 0;
    }
    else{
        precisionMask = pow(10,p);
    }
}

template class NumericParameter<int>;
template class NumericParameter<floatParamType>;
