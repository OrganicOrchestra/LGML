/*
  ==============================================================================

    IntParameter.cpp
    Created: 8 Mar 2016 1:22:23pm
    Author:  bkupe

  ==============================================================================
*/

#include "IntParameter.h"
#include "IntSliderUI.h"

IntParameter::IntParameter(const String & niceName, const int & initialValue, const int & minValue, const int & maxValue, bool enabled) :
	Parameter(niceName,enabled), minValue(minValue), maxValue(maxValue)
{
	setValue(initialValue);
}

IntSliderUI * IntParameter::createSlider()
{
	return new IntSliderUI(this);
}
