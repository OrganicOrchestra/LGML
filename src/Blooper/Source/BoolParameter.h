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

class BoolToggleUI;

class BoolParameter : public Parameter
{
public:
	BoolParameter(const String &niceName, const String &description, const bool &initialValue, bool enabled = true);
	~BoolParameter() {}

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

	//ui creation
	BoolToggleUI * createToggle();

    
    String toString() override{return String(value);}
    void fromString(const String & s,bool silentSet = false, bool force = false) override{setValue(s.getIntValue()!=0,silentSet,force);};
	JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(BoolParameter)
};



#endif  // BOOLPARAMETER_H_INCLUDED
