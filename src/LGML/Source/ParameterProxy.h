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
#include "StringParameter.h"

class ParameterProxy :
	public Parameter,
	public Parameter::Listener
{
public:
	ParameterProxy();
	virtual ~ParameterProxy();

	WeakReference<Parameter> linkedParam;

	StringParameter proxyAlias;

	bool isUpdatingLinkedParam;
	virtual void setValueInternal(var _value) override;

	// Inherited via Listener
	virtual void parameterValueChanged(Parameter * p) override;
	virtual void setLinkedParam(Parameter * p);


	void remove();

	// Inherited via Parameter
	virtual ControllableUI * createDefaultUI(Controllable * targetControllable = nullptr) override;


	var getJSONData();
	void loadJSONData(var data);

	class ParameterProxyListener
	{
	public:
		virtual ~ParameterProxyListener() {}
		virtual void linkedParamChanged(Parameter *) {};
		virtual void askForRemoveProxy(ParameterProxy *) {};
	};

	ListenerList<ParameterProxyListener> proxyListeners;
	void addParameterProxyListener(ParameterProxyListener* newListener) { proxyListeners.add(newListener); }
	void removeParameterProxyListener(ParameterProxyListener* listener) { proxyListeners.remove(listener); }

};



#endif  // PARAMETERPROXY_H_INCLUDED
