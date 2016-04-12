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

    void setValue(const bool &_value, bool silentSet = false, bool force = false)
    {
        if (!force && this->value == _value) return;
        this->value = _value;
        if (!silentSet) notifyValueChanged();
    }

    float getNormalizedValue() override
    {
        return value ? 1.f : 0.f;
    }

    //ui creation
    BoolToggleUI * createToggle();
    ControllableUI * createDefaultControllableEditor()override ;
    String toString() override{return String(value);}
    void fromString(const String & s,bool silentSet = false, bool force = false) override{setValue(s.getIntValue()!=0,silentSet,force);};
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(BoolParameter)
};



#endif  // BOOLPARAMETER_H_INCLUDED
