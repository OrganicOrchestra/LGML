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


#ifndef FLOATPARAMETER_H_INCLUDED
#define FLOATPARAMETER_H_INCLUDED



#include "Parameter.h"

class FloatParameter : public Parameter
{
public:
    FloatParameter(const String &niceName, const String &description, float initialValue=0.f,  float minValue = 0.f, float maxValue = 1.f, bool enabled = true);
    ~FloatParameter() {}

    void setValueInternal(var & _value) override;
    


   JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(FloatParameter)
};


#endif  // FLOATPARAMETER_H_INCLUDED
