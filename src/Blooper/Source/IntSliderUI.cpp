/*
  ==============================================================================

    IntSliderUI.cpp
    Created: 8 Mar 2016 3:46:34pm
    Author:  bkupe

  ==============================================================================
*/

#include "IntSliderUI.h"

IntSliderUI::IntSliderUI(Parameter * parameter) :
	FloatSliderUI(parameter),intParam((IntParameter *)parameter)
{

}

IntSliderUI::~IntSliderUI()
{
}

void IntSliderUI::setParamNormalizedValue(float value)
{
	intParam->setNormalizedValue(value);
}

float IntSliderUI::getParamNormalizedValue()
{
	return intParam->getNormalizedValue();
}
