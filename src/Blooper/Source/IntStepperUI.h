/*
  ==============================================================================

    IntStepperUI.h
    Created: 8 Mar 2016 3:46:43pm
    Author:  bkupe

  ==============================================================================
*/

#ifndef INTSTEPPERUI_H_INCLUDED
#define INTSTEPPERUI_H_INCLUDED

#include "ParameterUI.h"

class IntStepperUI : public ParameterUI, public Component
{
public:
	IntStepperUI(Parameter * parameter);
	~IntStepperUI();

protected:
	void parameterValueChanged(Parameter *) override  { repaint(); } ;


	JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(IntStepperUI)
};


#endif  // INTSTEPPERUI_H_INCLUDED
