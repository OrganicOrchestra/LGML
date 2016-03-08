/*
  ==============================================================================

    ControllableContainer.cpp
    Created: 8 Mar 2016 1:15:36pm
    Author:  bkupe

  ==============================================================================
*/

#include "ControllableContainer.h"

ControllableContainer::ControllableContainer(const String & shortName) : shortName(shortName)
{

}

void ControllableContainer::removeControllable(Controllable * c)
{
	controllables.removeObject(c);
}

Controllable * ControllableContainer::getControllableByName(const String & shortName)
{
	for (auto &c : controllables)
	{
		if (c->shortName == shortName) return c;
	}

	return nullptr;
}
