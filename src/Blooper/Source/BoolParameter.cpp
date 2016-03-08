/*
  ==============================================================================

    BoolParameter.cpp
    Created: 8 Mar 2016 1:22:15pm
    Author:  bkupe

  ==============================================================================
*/

#include "BoolParameter.h"

BoolParameter::BoolParameter(const String & shortName, const bool & initialValue, bool enabled) :
	Parameter(shortName, enabled)
{
	setValue(initialValue);
}
