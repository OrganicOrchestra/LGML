/*
  ==============================================================================

    FloatParameter.h
    Created: 8 Mar 2016 1:22:10pm
    Author:  bkupe

  ==============================================================================
*/

#ifndef FLOATPARAMETER_H_INCLUDED
#define FLOATPARAMETER_H_INCLUDED



#include "Parameter.h"

class FloatSliderUI;

class FloatParameter : public Parameter
{
public:
	FloatParameter(const String &niceName, const String &description, const float &initialValue, const float &minValue = 0, const float &maxValue = 1, bool enabled = true);
	~FloatParameter() {}

	float minValue;
	float maxValue;
	float value;

	void setValue(const float &value, bool silentSet = false, bool force = false)
	{
		if (!force && this->value == value) return;
		this->value = jlimit<float>(minValue, maxValue, value);
		if (!silentSet) notifyValueChanged();
	}

	void setNormalizedValue(const float &normalizedValue, bool silentSet = false, bool force = false)
	{
		setValue(jmap<float>(normalizedValue, minValue, maxValue), silentSet, force);
	}

	float getNormalizedValue() override
	{
		return jmap<float>(value, minValue, maxValue, 0, 1);
	}

	FloatSliderUI * createSlider();

	JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(FloatParameter)
};


#endif  // FLOATPARAMETER_H_INCLUDED
