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
#include "ControllableContainer.h"



class ParameterProxy :
  public StringParameter,
	private Parameter::Listener,
  public Controllable::Listener,
  private ControllableContainerListener
{
public:
  ParameterProxy(const String & niceName,const String & desc,Parameter * ref=nullptr,ControllableContainer * root=nullptr);
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

  
private:
  void controllableAdded(ControllableContainer *,Controllable * /*notifier*/) override;
  void controllableRemoved(Controllable * ) override;
  bool resolveAddress();

};



#endif  // PARAMETERPROXY_H_INCLUDED
