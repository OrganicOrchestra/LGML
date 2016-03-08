/*
  ==============================================================================

    Controllable.cpp
    Created: 8 Mar 2016 1:08:56pm
    Author:  bkupe

  ==============================================================================
*/

#include "Controllable.h"

Controllable::Controllable(const String & shortName, bool enabled) : 
	shortName(shortName)
{
	setEnabled(enabled);
}
