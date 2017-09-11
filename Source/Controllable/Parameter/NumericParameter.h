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
    NumericParameter(const String &niceName, const String &description, const T initialValue=0, const T minimumValue = 0, const T maximumValue = 1, bool enabled = true);
  ~NumericParameter() {};

    void setValueInternal(var & _value) override;
  
  void setNormalizedValue(const T  normalizedValue, bool silentSet=false, bool force=false);

  float getNormalizedValue() ;
  
  DECLARE_PARAM_TYPE(NumericParameter)
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(NumericParameter)
};

typedef NumericParameter<int> IntParameter;
typedef NumericParameter<double> FloatParameter;

// template implementation

template<class T>
float NumericParameter<T>::getNormalizedValue() {
  if((T)minimumValue==(T)maximumValue){
    return 0.0;
  }
  else
    return jmap<T>((T)value, (T)minimumValue, (T)maximumValue, (T)0, (T)1);
}

template<class T>
void NumericParameter<T>::setNormalizedValue(const T  normalizedValue, bool silentSet, bool force)
{
  setValue(jmap<T>(normalizedValue, (T)minimumValue, (T)maximumValue), silentSet, force);
}

#endif  // INTPARAMETER_H_INCLUDED
