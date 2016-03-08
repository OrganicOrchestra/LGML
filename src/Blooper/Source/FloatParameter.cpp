/*
  ==============================================================================

    FloatParameter.cpp
    Created: 8 Mar 2016 1:22:10pm
    Author:  bkupe

  ==============================================================================
*/

#include "FloatParameter.h"
#include "FloatSliderUI.h"

FloatParameter::FloatParameter(const String & niceName, const float & initialValue, const float & minValue, const float & maxValue, bool enabled) :
	Parameter(niceName,enabled), minValue(minValue),maxValue(maxValue)
{
	setValue(initialValue);
}

  FloatSliderUI * FloatParameter::createSlider()
  {
	  return new FloatSliderUI(this);
  }
