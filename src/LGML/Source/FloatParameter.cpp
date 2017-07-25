/*
  ==============================================================================

    FloatParameter.cpp
    Created: 8 Mar 2016 1:22:10pm
    Author:  bkupe

  ==============================================================================
*/

#include "FloatParameter.h"



FloatParameter::FloatParameter(const String & niceName, const String &description, const float & initialValue, const float & minValue, const float & maxValue, bool enabled) :
    Parameter(Type::FLOAT, niceName, description, (float)initialValue, (float)minValue, (float)maxValue, enabled)
{

}

void FloatParameter::setValueInternal(var & _value)
{
    value = jlimit<double>(minimumValue, maximumValue, _value);
}



