/*
  ==============================================================================

    Trigger.cpp
    Created: 8 Mar 2016 1:09:29pm
    Author:  bkupe

  ==============================================================================
*/

#include "Trigger.h"

Trigger::Trigger(const String & niceName, const String &description, bool enabled) :
	Controllable(niceName, description, enabled)
{
}
