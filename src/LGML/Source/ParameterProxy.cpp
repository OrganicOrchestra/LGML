/*
  ==============================================================================

    ParameterProxy.cpp
    Created: 31 May 2016 12:28:42pm
    Author:  bkupe

  ==============================================================================
*/

#include "ParameterProxy.h"

ParameterProxy::ParameterProxy(Parameter * _linkedParam) :
	linkedParam(_linkedParam),
	Parameter(PROXY,_linkedParam->niceName,_linkedParam->description,_linkedParam->value,_linkedParam->minimumValue,_linkedParam->maximumValue,_linkedParam->enabled),
	isUpdatingLinkedParam(false)
{
	linkedParam->addParameterListener(this);
}

ParameterProxy::~ParameterProxy()
{
	linkedParam->removeParameterListener(this);
}

void ParameterProxy::setValueInternal(var _value)
{
	if (linkedParam != nullptr && !isUpdatingLinkedParam)
	{
		isUpdatingLinkedParam = true; //avoid infinite cycling setValue between proxy and linkedParam
		linkedParam->setValue(_value);
		isUpdatingLinkedParam = false;
	} else
	{
		value = _value;
	}
}

void ParameterProxy::parameterValueChanged(Parameter * p)
{
	setValue(p->value); //should be silent ?
}

ControllableUI * ParameterProxy::createDefaultUI()
{
	if (linkedParam == nullptr) return nullptr;
	return linkedParam->createDefaultUI();
}
