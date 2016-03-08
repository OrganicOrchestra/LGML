/*
  ==============================================================================

	ControllableContainer.h
	Created: 8 Mar 2016 1:15:36pm
	Author:  bkupe

  ==============================================================================
*/

#ifndef CONTROLLABLECONTAINER_H_INCLUDED
#define CONTROLLABLECONTAINER_H_INCLUDED

#include "Controllable.h"
#include "FloatParameter.h"
#include "IntParameter.h"
#include "BoolParameter.h"
#include "Trigger.h"

class ControllableContainer
{
public:
	ControllableContainer(const String &shortName);
	~ControllableContainer();

	String shortName;
	OwnedArray<Controllable> controllables;

	FloatParameter * addFloatParameter(const String &shortName, const float &initialValue, const float &minValue = 0, const float &maxValue = 1, const bool &enabled = true);
	IntParameter * addIntParameter(const String &shortName, const int &initialValue, const int &minValue, const int &maxValue, const bool &enabled = true);
	BoolParameter * addBoolParameter(const String &shortName, const bool &value, const bool &enabled = true);
	Trigger * addTrigger(const String &shortName, const bool &enabled = true);

	void removeControllable(Controllable * c);
	Controllable * getControllableByName(const String &name);


public:
	//Listener
	class  Listener
	{
	public:
		/** Destructor. */
		virtual ~Listener() {}
		virtual void controllableAdded(Controllable * c) = 0;
		virtual void controllableRemoved(Controllable * c) = 0;
	};

	ListenerList<Listener> listeners;
	void addListener(Listener* newListener) { listeners.add(newListener); }
	void removeListener(Listener* listener) { listeners.remove(listener); }
};


#endif  // CONTROLLABLECONTAINER_H_INCLUDED
