/*
  ==============================================================================

    ParameterProxy.cpp
    Created: 31 May 2016 12:28:42pm
    Author:  bkupe

  ==============================================================================
*/

#include "ParameterProxy.h"
#include "ParameterProxyUI.h"
#include "ControllableContainer.h"

ParameterProxy::ParameterProxy() :
	linkedParam(nullptr),
	Parameter(PROXY, "proxy", "proxy description", 0, 0, 1),
	isUpdatingLinkedParam(false),
	proxyAlias("alias","Proxy Alias\nThis will be used to set the OSC control address","proxy")
{
	proxyAlias.addParameterListener(this);
}

ParameterProxy::~ParameterProxy()
{
	setLinkedParam(nullptr);
}

void ParameterProxy::setValueInternal(var & _value)
{
	Parameter::setValueInternal(_value);

	if (linkedParam != nullptr && !isUpdatingLinkedParam)
	{
		isUpdatingLinkedParam = true; //avoid infinite cycling setValue between proxy and linkedParam
		linkedParam->setValue(_value);
		isUpdatingLinkedParam = false;
	}
}

void ParameterProxy::parameterValueChanged(Parameter * p)
{
	if (p && (p == linkedParam))
	{
		setValue(p->value); //should be silent ?
	} else if (p == &proxyAlias)
	{
		setNiceName(p->stringValue());
	}
}

void ParameterProxy::setLinkedParam(Parameter * p)
{
	if (linkedParam == p) return;
	if (linkedParam != nullptr)
	{
		linkedParam->removeParameterListener(this);
	}
	linkedParam = p;

	if (linkedParam != nullptr)
	{
		linkedParam->addParameterListener(this);
		description = String("Proxy Param for "+linkedParam->getControlAddress(parentContainer));
		setRange(linkedParam->minimumValue, linkedParam->maximumValue);
		setValue(linkedParam->value, true, true);

	}

	proxyListeners.call(&ParameterProxyListener::linkedParamChanged, linkedParam);
}

void ParameterProxy::remove()
{
	DBG("dispatch askForRemove");
	proxyListeners.call(&ParameterProxyListener::askForRemoveProxy, this);
}

ControllableUI * ParameterProxy::createDefaultUI(Controllable * targetControllable)
{
	if (targetControllable == nullptr) targetControllable = this;
	return new ParameterProxyUI(dynamic_cast<ParameterProxy *>(targetControllable));
}

var ParameterProxy::getJSONData()
{
	var data(new DynamicObject());
	data.getDynamicObject()->setProperty("alias", proxyAlias.stringValue());
	if (linkedParam != nullptr) data.getDynamicObject()->setProperty("linkedParam", linkedParam->getControlAddress(parentContainer));
	return data;
}

void ParameterProxy::loadJSONData(var data)
{
	proxyAlias.setValue(data.getDynamicObject()->getProperty("alias").toString());
	if (parentContainer != nullptr)
	{
		String relAddress = data.getDynamicObject()->getProperty("linkedParam").toString();
		if(relAddress.isNotEmpty()) setLinkedParam((Parameter *)parentContainer->getControllableForAddress(relAddress));
	}
}
