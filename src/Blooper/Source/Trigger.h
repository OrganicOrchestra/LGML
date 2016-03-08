/*
  ==============================================================================

    Trigger.h
    Created: 8 Mar 2016 1:09:29pm
    Author:  bkupe

  ==============================================================================
*/

#ifndef TRIGGER_H_INCLUDED
#define TRIGGER_H_INCLUDED

#include "Controllable.h"

class Trigger : public Controllable
{
public:
	Trigger(const String &shortName, bool enabled = true);

	void trigger()
	{
		if (enabled) listeners.call(&Listener::triggerTriggered, this);
	}

public:
	//Listener
	class  Listener
	{
	public:
		/** Destructor. */
		virtual ~Listener() {}
		virtual void triggerTriggered(Trigger * p) = 0;
	};

	ListenerList<Listener> listeners;
	void addListener(Listener* newListener) { listeners.add(newListener); }
	void removeListener(Listener* listener) { listeners.remove(listener); }

};



#endif  // TRIGGER_H_INCLUDED
