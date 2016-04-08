/*
  ==============================================================================

    ParameterUI.h
    Created: 8 Mar 2016 3:48:44pm
    Author:  bkupe

  ==============================================================================
*/

#ifndef PARAMETERUI_H_INCLUDED
#define PARAMETERUI_H_INCLUDED

#include "Parameter.h"
#include "ControllableUI.h"

class ParameterUI : public Parameter::Listener, public ControllableUI
{
public:
    ParameterUI(Parameter * parameter);
    virtual ~ParameterUI();

    WeakReference<Parameter> parameter;

protected:
    // Inherited via Listener
    virtual void parameterValueChanged(Parameter * p) override;


    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(ParameterUI)
};


#endif  // PARAMETERUI_H_INCLUDED
