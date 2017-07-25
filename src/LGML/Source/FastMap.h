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
#include "ParameterProxy.h"

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
public ParameterProxy::ParameterProxyListener
{
public:
	FastMap();
	virtual ~FastMap();

	BoolParameter * enabledParam;

	FloatParameter * minInputVal;
	FloatParameter * maxInputVal;
	FloatParameter * minOutputVal;
	FloatParameter * maxOutputVal;
	BoolParameter * invertParam;

	String ghostAddress; //for ghosting if parameter not found

	ParameterProxy * referenceIn;
	ParameterProxy *  referenceOut;

  

	bool isInRange; //memory for triggering
	void process();

	void setGhostAddress(const String &address);

	void remove();



  // inherited from proxy listener
   void linkedParamValueChanged(ParameterProxy *) override;
		 void linkedParamChanged(ParameterProxy *) override;
	ListenerList<FastMapListener> fastMapListeners;
	void addFastMapListener(FastMapListener* newListener) { fastMapListeners.add(newListener); }
	void removeFastMapListener(FastMapListener* listener) { fastMapListeners.remove(listener); }



	JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(FastMap);
};


#endif  // FASTMAP_H_INCLUDED
