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

	ParameterProxy * referenceIn;
	ParameterProxy *  referenceOut;

  void onContainerParameterChanged(Parameter *)override;

	bool isInRange; //memory for triggering
	void process();


  // inherited from proxy listener
   void linkedParamValueChanged(ParameterProxy *) override;
  void linkedParamChanged(ParameterProxy *) override;

	JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(FastMap);
};


#endif  // FASTMAP_H_INCLUDED
