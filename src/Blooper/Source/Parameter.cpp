/*
  ==============================================================================

    Parameter.cpp
    Created: 8 Mar 2016 1:08:19pm
    Author:  bkupe

  ==============================================================================
*/

#include "Parameter.h"

Parameter::Parameter(const Type &type, const String &niceName, const String &description, bool enabled) :
	Controllable(type, niceName, description, enabled)
{

}