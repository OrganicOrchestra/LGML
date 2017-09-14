/* Copyright © Organic Orchestra, 2017
*
* This file is part of LGML.  LGML is a software to manipulate sound in realtime
*
* This program is free software; you can redistribute it and/or modify
* it under the terms of the GNU General Public License as published by
* the Free Software Foundation (version 3 of the License).
*
* This program is distributed in the hope that it will be useful,
* but WITHOUT ANY WARRANTY; without even the implied warranty of
* MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
*
*/


#ifndef PARAMETERPROXY_H_INCLUDED
#define PARAMETERPROXY_H_INCLUDED

#include "Parameter.h"
#include "StringParameter.h"
#include "../ControllableContainer.h"



class ParameterProxy :
  public StringParameter,
	private Parameter::Listener,
  public Controllable::Listener,
private ControllableContainer::Listener
{
public:
  ParameterProxy(const String & niceName,const String & desc=String::empty,Parameter * ref=nullptr,ControllableContainer * root=nullptr);
	virtual ~ParameterProxy();


	WeakReference<Parameter> linkedParam;
  ControllableContainer * rootOfProxy;

  void setRoot(ControllableContainer * );
  Parameter * get();
  void tryToSetValue(var _value,bool silentSet,bool force )override;
  void setValueInternal(var & _value) override;

	// Inherited via Listener
  void parameterValueChanged(Parameter * p) override;
   
	 void setParamToReferTo(Parameter * p);


	class ParameterProxyListener
	{
	public:
		virtual ~ParameterProxyListener() {}
    virtual void linkedParamValueChanged(ParameterProxy *) {};
		virtual void linkedParamChanged(ParameterProxy *) {};
	};

	ListenerList<ParameterProxyListener> proxyListeners;
	void addParameterProxyListener(ParameterProxyListener* newListener) { proxyListeners.add(newListener); }
	void removeParameterProxyListener(ParameterProxyListener* listener) { proxyListeners.remove(listener); }

  ControllableContainer * getRoot();

  DECLARE_OBJ_TYPE(ParameterProxy)
private:
  void controllableAdded(ControllableContainer *,Controllable * /*notifier*/) override;
  void controllableRemoved(Controllable * ) override;
  bool resolveAddress();

};



#endif  // PARAMETERPROXY_H_INCLUDED
