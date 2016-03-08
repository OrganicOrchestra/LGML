/*
  ==============================================================================

    BoolParameter.cpp
    Created: 8 Mar 2016 1:22:15pm
    Author:  bkupe

  ==============================================================================
*/

#include "BoolParameter.h"

BoolParameter::BoolParameter(const String & niceName, const bool & initialValue, bool enabled) :
	Parameter(niceName, enabled)
{
	setValue(initialValue);
}
