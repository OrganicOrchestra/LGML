/*
  ==============================================================================

    ControllableContainer.cpp
    Created: 8 Mar 2016 1:15:36pm
    Author:  bkupe

  ==============================================================================
*/

#include "ControllableContainer.h"

ControllableContainer::ControllableContainer(const String & niceName) : niceName(niceName)
{
	setNiceName(niceName);
}

ControllableContainer::~ControllableContainer()
{
	controllables.clear();
}

FloatParameter * ControllableContainer::addFloatParameter(const String & niceName, const float & initialValue, const float & minValue, const float & maxValue, const bool & enabled)
{
	if (getControllableByName(niceName) != nullptr)
	{
		DBG("ControllableContainer::add parameter, short Name already exists : " + niceName);
		return nullptr;
	}

	FloatParameter * p = new FloatParameter(niceName, initialValue, minValue, maxValue, enabled);
	controllables.add(p);
	listeners.call(&Listener::controllableAdded, p);
	return p;
}

IntParameter * ControllableContainer::addIntParameter(const String & niceName, const int & initialValue, const int & minValue, const int & maxValue, const bool & enabled)
{
	if (getControllableByName(niceName) != nullptr)
	{
		DBG("ControllableContainer::add parameter, short Name already exists : " + niceName);
		return nullptr;
	}

	IntParameter * p = new IntParameter(niceName, initialValue, minValue, maxValue, enabled);
	controllables.add(p);
	listeners.call(&Listener::controllableAdded, p);
	return p;
}

BoolParameter * ControllableContainer::addBoolParameter(const String & niceName, const bool & value, const bool & enabled)
{
	if (getControllableByName(niceName) != nullptr)
	{
		DBG("ControllableContainer::add parameter, short Name already exists : " + niceName);
		return nullptr;
	}

	BoolParameter * p = new BoolParameter(niceName, value, enabled);
	controllables.add(p);
	listeners.call(&Listener::controllableAdded, p);
	return p;
}

Trigger * ControllableContainer::addTrigger(const String & niceName, const bool & enabled)
{
	if (getControllableByName(niceName) != nullptr)
	{
		DBG("ControllableContainer::add trigger, short Name already exists : " + niceName);
		return nullptr;
	}

	Trigger * t = new Trigger(niceName, enabled);
	controllables.add(t);
	listeners.call(&Listener::controllableAdded, t);
	return t;
}

void ControllableContainer::removeControllable(Controllable * c)
{
	listeners.call(&Listener::controllableRemoved, c);
	controllables.removeObject(c);
}

Controllable * ControllableContainer::getControllableByName(const String & niceName)
{
	for (auto &c : controllables)
	{
		if (c->niceName == niceName) return c;
	}

	return nullptr;
}
