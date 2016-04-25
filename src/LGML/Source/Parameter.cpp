/*
  ==============================================================================

    Parameter.cpp
    Created: 8 Mar 2016 1:08:19pm
    Author:  bkupe

  ==============================================================================
*/

#include "Parameter.h"

Parameter::Parameter(const Type &type, const String &niceName, const String &description, var initialValue, var minValue = var(), var maxValue = var(), bool enabled) :
    Controllable(type, niceName, description, enabled)
{
	minimumValue = minValue;
	maximumValue = maxValue;
	defaultValue = initialValue;
	
	isOverriden = false;
	isPresettable = true;
	
	resetValue();
}

void Parameter::resetValue()
{
	isOverriden = false;
	setValue(defaultValue);
}

void Parameter::setValue(var _value, bool silentSet, bool force)
{
	if (!force && this->value == _value) return;
	setValueInternal(_value);

	if(_value != defaultValue) isOverriden = true;

	if (!silentSet) notifyValueChanged();
}

void Parameter::setValueInternal(var _value) //to override by child classes
{
	value = _value;
	
}

void Parameter::setNormalizedValue(const float & normalizedValue, bool silentSet, bool force)
{
	setValue(jmap<float>(normalizedValue, (float)minimumValue, (float)maximumValue), silentSet, force);
}

float Parameter::getNormalizedValue() {
	return jmap<float>((float)value, (float)minimumValue, (float)maximumValue, 0.f, 1.f);
}

void Parameter::notifyValueChanged() { 
	listeners.call(&Listener::parameterValueChanged, this); 
}
