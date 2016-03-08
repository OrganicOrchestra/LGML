/*
  ==============================================================================

    FloatRangeParameter.h
    Created: 8 Mar 2016 6:49:15pm
    Author:  bkupe

  ==============================================================================
*/

#ifndef FLOATRANGEPARAMETER_H_INCLUDED
#define FLOATRANGEPARAMETER_H_INCLUDED


#include "Parameter.h"

class FloatRangeParameter : public Parameter
{
public:
	FloatRangeParameter(const String &niceName, const int &initialValueMin, const int &initialValueMax, const int &minValue = 0, const int &maxValue = 1, bool enabled = true);
	~FloatRangeParameter() {}

	float minValue;
	float maxValue;

	float valueMin;
	float valueMax;

	void setValuesMinMax(const float &valueMin, const float &valueMax, bool silentSet = false, bool force = false)
	{
		if (!force && this->valueMin == valueMin && this->valueMax == valueMax) return;
		this->valueMin = jlimit<float>(minValue, maxValue, valueMin);
		this->valueMax = jlimit<float>(valueMin, maxValue, valueMax);
		if (!silentSet) notifyValueChanged();
	}

	void setValuesMaxMin(const float &valueMin, const float &valueMax, bool silentSet = false, bool force = false)
	{
		if (!force && this->valueMin == valueMin && this->valueMax == valueMax) return;
		this->valueMax = jlimit<float>(minValue, maxValue, valueMax);
		this->valueMin = jlimit<float>(minValue, valueMax, valueMin);
		if (!silentSet) notifyValueChanged();
	}

	float getNormalizedValueMin()
	{
		return jmap<float>(valueMin, minValue, maxValue, 0, 1);
	}

	float getNormalizedValueMax()
	{
		return jmap<float>(valueMax, minValue, maxValue, 0, 1);
	}


	JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(FloatRangeParameter)
};





#endif  // FLOATRANGEPARAMETER_H_INCLUDED
