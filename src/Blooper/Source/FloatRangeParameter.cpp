/*
  ==============================================================================

    FloatRangeParameter.cpp
    Created: 8 Mar 2016 6:49:15pm
    Author:  bkupe

  ==============================================================================
*/

#include "FloatRangeParameter.h"

FloatRangeParameter::FloatRangeParameter(const String & niceName, const String &description, const int & initialValueMin, const int &initialValueMax, const int & minValue, const int & maxValue, bool enabled) :
	Parameter(Type::RANGE,niceName, description, enabled), minValue(minValue), maxValue(maxValue)
{
	//base set values to min and max so they don't overlap when setting with setValues
	valueMin = minValue;
	valueMax = maxValue;
	
	setValuesMinMax(initialValueMin,initialValueMax);
}