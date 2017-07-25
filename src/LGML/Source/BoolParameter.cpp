/*
  ==============================================================================

    BoolParameter.cpp
    Created: 8 Mar 2016 1:22:15pm
    Author:  bkupe

  ==============================================================================
*/

#include "BoolParameter.h"


BoolParameter::BoolParameter(const String & niceName, const String &description, bool initialValue, bool enabled) :
    Parameter(Type::BOOL, niceName, description, initialValue, false, true, enabled),
	invertVisuals(false)
{
    setValue(initialValue);
}

