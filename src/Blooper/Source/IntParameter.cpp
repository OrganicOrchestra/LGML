/*
  ==============================================================================

    IntParameter.cpp
    Created: 8 Mar 2016 1:22:23pm
    Author:  bkupe

  ==============================================================================
*/

#include "IntParameter.h"

IntParameter::IntParameter(const String & shortName, const int & initialValue, const int & minValue, const int & maxValue, bool enabled) :
	Parameter(shortName,enabled), minValue(minValue), maxValue(maxValue)
{
	setValue(initialValue);
}
