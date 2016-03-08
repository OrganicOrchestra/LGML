/*
  ==============================================================================

    Parameter.h
    Created: 8 Mar 2016 1:08:19pm
    Author:  bkupe

  ==============================================================================
*/

#ifndef PARAMETER_H_INCLUDED
#define PARAMETER_H_INCLUDED

#include "Controllable.h"

class Parameter : public Controllable
{
public:
	class Parameter(const String &shortName, bool enabled = true);
	virtual float getNormalizedValue() = 0;


protected:
	void notifyValueChanged() { listeners.call(&Listener::parameterValueChanged, this); }


public:
	//Listener
	class  Listener
	{
	public:
		/** Destructor. */
		virtual ~Listener() {}
		virtual void parameterValueChanged(Parameter * p) = 0;
	};

	ListenerList<Listener> listeners;
	void addListener(Listener* newListener) { listeners.add(newListener); }
	void removeListener(Listener* listener) { listeners.remove(listener); }

	
};


#endif  // PARAMETER_H_INCLUDED
