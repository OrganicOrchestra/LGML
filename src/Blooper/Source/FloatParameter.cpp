/*
  ==============================================================================

    FloatParameter.cpp
    Created: 8 Mar 2016 1:22:10pm
    Author:  bkupe

  ==============================================================================
*/

#include "FloatParameter.h"
#include "FloatSliderUI.h"

FloatParameter::FloatParameter(const String & niceName, const int & initialValue, const int & minValue, const int & maxValue, bool enabled) :
	Parameter(niceName,enabled), minValue(minValue),maxValue(maxValue)
{
	setValue(initialValue);
}

  FloatSliderUI * FloatParameter::createSlider()
  {
	  return new FloatSliderUI(this);
  }
