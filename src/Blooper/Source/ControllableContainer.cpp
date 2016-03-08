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

ControllableContainer::~ControllableContainer()
{
	controllables.clear();
}

FloatParameter * ControllableContainer::addFloatParameter(const String & shortName, const float & initialValue, const float & minValue, const float & maxValue, const bool & enabled)
{
	if (getControllableByName(shortName) != nullptr)
	{
		DBG("ControllableContainer::add parameter, short Name already exists : " + shortName);
		return;
	}

	FloatParameter * p = new FloatParameter(shortName, initialValue, minValue, maxValue, enabled);
	controllables.add(p);
	listeners.call(&Listener::controllableAdded, p);
	return p;
}

IntParameter * ControllableContainer::addIntParameter(const String & shortName, const int & initialValue, const int & minValue, const int & maxValue, const bool & enabled)
{
	if (getControllableByName(shortName) != nullptr)
	{
		DBG("ControllableContainer::add parameter, short Name already exists : " + shortName);
		return;
	}

	IntParameter * p = new IntParameter(shortName, initialValue, minValue, maxValue, enabled);
	controllables.add(p);
	listeners.call(&Listener::controllableAdded, p);
	return p;
}

BoolParameter * ControllableContainer::addBoolParameter(const String & shortName, const bool & value, const bool & enabled)
{
	if (getControllableByName(shortName) != nullptr)
	{
		DBG("ControllableContainer::add parameter, short Name already exists : " + shortName);
		return;
	}

	BoolParameter * p = new BoolParameter(shortName, value, enabled);
	controllables.add(p);
	listeners.call(&Listener::controllableAdded, p);
	return p;
}

Trigger * ControllableContainer::addTrigger(const String & shortName, const bool & enabled)
{
	if (getControllableByName(shortName) != nullptr)
	{
		DBG("ControllableContainer::add trigger, short Name already exists : " + shortName);
		return;
	}

	Trigger * t = new Trigger(shortName, enabled);
	controllables.add(t);
	listeners.call(&Listener::controllableAdded, t);
	return t;
}

void ControllableContainer::removeControllable(Controllable * c)
{
	listeners.call(&Listener::controllableRemoved, c);
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
