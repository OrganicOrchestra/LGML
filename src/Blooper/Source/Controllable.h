/*
  ==============================================================================

    Controllable.h
    Created: 8 Mar 2016 1:08:56pm
    Author:  bkupe

  ==============================================================================
*/

#ifndef CONTROLLABLE_H_INCLUDED
#define CONTROLLABLE_H_INCLUDED

#include "JuceHeader.h"

class Controllable
{
public:
	Controllable(const String &shortName, bool enabled = true);

	bool enabled;
	String shortName;

	void setEnabled(bool value, bool silentSet = false, bool force = false)
	{
		if (!force && value == enabled) return;

		enabled = value;
		if(!silentSet) listeners.call(&Listener::controllableStateChanged, this);
		
	}

	class  Listener
	{
	public:
		/** Destructor. */
		virtual ~Listener() {}
		virtual void controllableStateChanged(Controllable * c) = 0;
	};

	ListenerList<Listener> listeners;
	void addListener(Listener* newListener) { listeners.add(newListener); }
	void removeListener(Listener* listener) { listeners.remove(listener); }
};

#endif  // CONTROLLABLE_H_INCLUDED
