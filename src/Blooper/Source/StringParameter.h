/*
  ==============================================================================

    StringParameter.h
    Created: 9 Mar 2016 12:29:30am
    Author:  bkupe

  ==============================================================================
*/

#ifndef STRINGPARAMETER_H_INCLUDED
#define STRINGPARAMETER_H_INCLUDED


#include "Parameter.h"

class StringParameterUI;

class StringParameter : public Parameter
{
public:
	StringParameter(const String &niceName, const String &initialValue, bool enabled);
	String value;

	void setValue(const String &value, bool silentSet = false, bool force = false)
	{
		if (this->value == value && !force) return;
		this->value = value;
		if (!silentSet) notifyValueChanged();
	}

	StringParameterUI * getUI();

	JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(StringParameter)
};



#endif  // STRINGPARAMETER_H_INCLUDED
