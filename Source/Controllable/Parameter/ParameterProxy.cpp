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


#include "ParameterProxy.h"
#include "../ControllableContainer.h"
#include "../../Engine.h"

#include "ParameterFactory.h"
REGISTER_PARAM_TYPE (ParameterProxy)

ParameterProxy::ParameterProxy (const String& niceName, const String& desc, Parameter* ref, ControllableContainer* root) :
    StringParameter (niceName, desc),
    linkedParam (ref),
    rootOfProxy (nullptr)
{
    setRoot (root);


}

ParameterProxy::~ParameterProxy()
{
    if (auto r = getRoot())r->removeControllableContainerListener (this);

    if (linkedParam != nullptr)
    {
        linkedParam->removeControllableListener (this);
        linkedParam->removeParameterListener (this);
    }
}

bool ParameterProxy::isMappable()
{
    return true;
}

void ParameterProxy::setRoot (ControllableContainer* r)
{
    if (rootOfProxy != nullptr)rootOfProxy->removeControllableContainerListener (this);

    rootOfProxy = r;

    resolveAddress();

}
void ParameterProxy::tryToSetValue (const var & _value, bool silentSet, bool force,Parameter::Listener * notifier )
{

    if (_value.isString())
    {
        StringParameter::tryToSetValue (_value, silentSet, force,notifier);
    }
    else if (linkedParam)
    {
        //WIP : polymorphic set value not supported
        jassertfalse;
        linkedParam->tryToSetValue (_value, silentSet, force,notifier);
    }
};
void ParameterProxy::setValueInternal (const var& _value)
{
    if (_value.isString())
    {
        StringParameter::setValueInternal (_value);

        if (auto* root = getRoot())
        {
            if (!resolveAddress() && stringValue().isNotEmpty())
            {
                root->addControllableContainerListener (this);
            }
            else
            {
                root->removeControllableContainerListener (this);
            }
        }
    }
    else if (linkedParam)
    {
        //WIP : polymorphic set value not supported
        jassertfalse;
        linkedParam->setValueInternal (_value);
    }
}


void ParameterProxy::parameterValueChanged (Parameter* p,Parameter::Listener * notifier)
{
    jassert (p == linkedParam);
    proxyListeners.call (&ParameterProxyListener::linkedParamValueChanged, this);
}

void ParameterProxy::parameterRangeChanged (Parameter* p)
{
    jassert (p == linkedParam);
    proxyListeners.call (&ParameterProxyListener::linkedParamRangeChanged, this);
}


Parameter* ParameterProxy::get()
{
    return linkedParam.get();
}
void ParameterProxy::setParamToReferTo (Parameter* p)
{

    String targetAddress = p ? p->getControlAddress (getRoot()) : String::empty;

    if ( targetAddress != stringValue())
    {
        setValue (targetAddress);
    }
    else
    {
        if(linkedParam == p) return;
        if (linkedParam != nullptr)
        {
            linkedParam->removeParameterListener (this);
            linkedParam->removeControllableListener (this);
        }

        if (p == this)
        {
            jassertfalse;
            DBG ("try to auto reference proxy");
            linkedParam = nullptr;
        }
        else
        {
            linkedParam = p;
        }

        if (linkedParam != nullptr)
        {
            linkedParam->addParameterListener (this);
            linkedParam->addControllableListener (this);
        }

        proxyListeners.call (&ParameterProxyListener::linkedParamChanged, this);
    }
}



ControllableContainer* ParameterProxy::getRoot()
{
    return (rootOfProxy ? rootOfProxy : getEngine());
}


bool ParameterProxy::resolveAddress()
{
    if (stringValue().isNotEmpty())
    {
        auto p = Parameter::fromControllable (getRoot()->getControllableForAddress (stringValue()));

        setParamToReferTo (p);
    }
    else
    {
        setParamToReferTo (nullptr);
    }

    return linkedParam != nullptr;
}

void ParameterProxy::controllableAdded (ControllableContainer*, Controllable* c)
{
    jassert (linkedParam == nullptr);

    if (c->getControlAddress() == stringValue())
    {
        setParamToReferTo (Parameter::fromControllable (c));
    }

}

void ParameterProxy::controllableRemoved (Controllable* c)
{
    if (c == (Controllable*)linkedParam || !linkedParam.get())
    {
        setParamToReferTo (nullptr);
    }

};
