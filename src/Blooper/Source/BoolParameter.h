/*
  ==============================================================================

    BoolParameter.h
    Created: 8 Mar 2016 1:22:15pm
    Author:  bkupe

  ==============================================================================
*/

#ifndef BOOLPARAMETER_H_INCLUDED
#define BOOLPARAMETER_H_INCLUDED

#include "Parameter.h"


class BoolParameter : public Parameter
{
public:
	BoolParameter(const String &shortName, const bool &initialValue, bool enabled = true);

	bool value;

	void setValue(const bool &value, bool silentSet = false, bool force = false)
	{
		if (!force && this->value == value) return;
		this->value = value;
		if (!silentSet) notifyValueChanged();
	}

	float getNormalizedValue() override
	{
		return value ? 1 : 0;
	}
};



#endif  // BOOLPARAMETER_H_INCLUDED
