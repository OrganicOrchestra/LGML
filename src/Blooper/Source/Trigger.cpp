/*
  ==============================================================================

    Trigger.cpp
    Created: 8 Mar 2016 1:09:29pm
    Author:  bkupe

  ==============================================================================
*/

#include "Trigger.h"

Trigger::Trigger(const String & shortName, bool enabled) :
	Controllable(shortName,enabled)
{
}
