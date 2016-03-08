/*
  ==============================================================================

    Controllable.cpp
    Created: 8 Mar 2016 1:08:56pm
    Author:  bkupe

  ==============================================================================
*/

#include "Controllable.h"

Controllable::Controllable(const String & niceName, bool enabled)
{
	setEnabled(enabled);
	setNiceName(niceName);
	DBG("Add controllable :" + niceName + " >> " + shortName);
}
