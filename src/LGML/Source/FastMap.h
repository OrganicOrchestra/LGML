/*
  ==============================================================================

    FastMap.h
    Created: 17 May 2016 6:05:27pm
    Author:  bkupe

  ==============================================================================
*/

#ifndef FASTMAP_H_INCLUDED
#define FASTMAP_H_INCLUDED

#include "ControllableContainer.h"
#include "ControlVariableReference.h"

class FastMap;

class FastMapListener
{
public:
    virtual ~FastMapListener(){}
	virtual void fastMapReferenceChanged(FastMap *) {};
	virtual void fastMapTargetChanged(FastMap *) {};

	virtual void askForRemoveFastMap(FastMap *) {};

	virtual void fastMapRemoved(FastMap *) {};
};

class FastMap :
	public ControllableContainer,
	public ControlVariableReferenceListener,
	public Controllable::Listener
{
public:
	FastMap();
	virtual ~FastMap();

	BoolParameter * enabledParam;

	FloatParameter * minInputVal;
	FloatParameter * maxInputVal;
	FloatParameter * minOutputVal;
	FloatParameter * maxOutputVal;

	ScopedPointer<ControlVariableReference> reference;
	Controllable * target;

	bool isInRange; //memory for triggering
	void process();

	void setReference(ControlVariableReference * r);
	void setTarget(Controllable * c);

	virtual var getJSONData() override;
	virtual void loadJSONDataInternal(var data) override;

	void remove();

	ListenerList<FastMapListener> fastMapListeners;
	void addFastMapListener(FastMapListener* newListener) { fastMapListeners.add(newListener); }
	void removeFastMapListener(FastMapListener* listener) { fastMapListeners.remove(listener); }

	virtual void referenceValueChanged(ControlVariableReference *) override;
	virtual void controllableRemoved(Controllable  *c) override;

	JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(FastMap);
};


#endif  // FASTMAP_H_INCLUDED
