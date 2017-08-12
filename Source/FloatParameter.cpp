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


#include "FloatParameter.h"



FloatParameter::FloatParameter(const String & niceName, const String &description,  float  initialValue,  float  minValue,  float maxValue, bool enabled) :
    Parameter(Type::FLOAT, niceName, description, (float)initialValue, (float)minValue, (float)maxValue, enabled)
{

}

void FloatParameter::setValueInternal(var & _value)
{
    value = jlimit<double>(minimumValue, maximumValue, _value);
}



