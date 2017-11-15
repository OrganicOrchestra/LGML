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
REGISTER_PARAM_TYPE_TEMPLATED (NumericParameter, int)
REGISTER_PARAM_TYPE_TEMPLATED (NumericParameter, double)





template<class T>
NumericParameter<T>::NumericParameter (const String& niceName, const String& description,
                                       const T  initialValue, const T  minValue, const T  maxValue,
                                       bool enabled) :
    MinMaxParameter ( niceName, description, var (initialValue), var (minValue), var (maxValue), enabled)
{
    if (std::is_same<T, int>::value)
    {
        jassert (value.isInt());
    }
    else if (std::is_same<T, double>::value)
    {
        jassert (value.isDouble());
    }
    else
    {
        // another Numeric parameter type?
        jassertfalse;
    }

}

template<class T>
void  NumericParameter<T>::setValueInternal (const var& _value)
{
    value = jlimit<T> ((T)minimumValue, (T) maximumValue, (T)_value);
}


template class NumericParameter<int>;
template class NumericParameter<double>;
