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
    private ParameterBase::Listener,
    public Controllable::Listener,
    private ControllableContainer::Listener
{
public:
    ParameterProxy (const String& niceName, const String& desc = "", ParameterBase* ref = nullptr, ControllableContainer* root = nullptr);
    virtual ~ParameterProxy();


    WeakReference<ParameterBase> linkedParam;
    ControllableContainer* rootOfProxy;

    void setRoot (ControllableContainer* );
    ParameterBase* getLinkedParam();
    void tryToSetValue (const var & _value, bool silentSet, bool force, ParameterBase::Listener * notifier=nullptr )override;
    void setValueInternal (const var& _value) override;
    bool isMappable() override;

    // Inherited via Listener
    void parameterValueChanged ( ParameterBase* p, ParameterBase::Listener * notifier) override;
    void parameterRangeChanged ( ParameterBase* ) override;
    
    void setParamToReferTo ( ParameterBase* p);


    class ParameterProxyListener
    {
    public:
        virtual ~ParameterProxyListener() {}
        virtual void linkedParamValueChanged (ParameterProxy*,ParameterBase::Listener *) {};
        virtual void linkedParamChanged (ParameterProxy*) {};
        virtual void linkedParamRangeChanged(ParameterProxy*) {};
    };

    ListenerList<ParameterProxyListener> proxyListeners;
    void addParameterProxyListener (ParameterProxyListener* newListener) { proxyListeners.add (newListener); }
    void removeParameterProxyListener (ParameterProxyListener* listener) { proxyListeners.remove (listener); }

    ControllableContainer* getRoot();

    DECLARE_OBJ_TYPE (ParameterProxy,"proxy parameter (can become any given parameter)")
private:
    void childControllableAdded (ControllableContainer*, Controllable* /*notifier*/) override;
    void controllableRemoved (Controllable* ) override;
    bool resolveAddress();
    WeakReference<ParameterProxy>::Master masterReference;
    friend class WeakReference<ParameterProxy>;

};



#endif  // PARAMETERPROXY_H_INCLUDED
