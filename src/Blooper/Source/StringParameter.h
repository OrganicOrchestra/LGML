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
    StringParameter(const String &niceName, const String &description, const String &initialValue, bool enabled=true);
    String value;

    void setValue(const String &_value, bool silentSet = false, bool force = false)
    {
        if (this->value == _value && !force) return;
        this->value = _value;
        if (!silentSet) notifyValueChanged();
    }

    StringParameterUI * createStringParameterUI();

    ControllableUI* createDefaultControllableEditor()override;
    String toString()override {return value;}
    void fromString(const String & s,bool silentSet = false, bool force = false) override{setValue(s,silentSet,force);};
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(StringParameter)
};



#endif  // STRINGPARAMETER_H_INCLUDED
