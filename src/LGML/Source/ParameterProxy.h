/*
  ==============================================================================

    ParameterProxy.h
    Created: 31 May 2016 12:28:42pm
    Author:  bkupe

  ==============================================================================
*/

#ifndef PARAMETERPROXY_H_INCLUDED
#define PARAMETERPROXY_H_INCLUDED

#include "Parameter.h"

class ParameterProxy : 
	public Parameter,
	public Parameter::Listener
{
public:
	ParameterProxy(Parameter * linkedParam);
	virtual ~ParameterProxy();

	WeakReference<Parameter> linkedParam;

	bool isUpdatingLinkedParam;

	virtual void setValueInternal(var _value) override;

	// Inherited via Listener
	virtual void parameterValueChanged(Parameter * p) override;


	// Inherited via Parameter
	virtual ControllableUI * createDefaultUI() override;

};



#endif  // PARAMETERPROXY_H_INCLUDED
