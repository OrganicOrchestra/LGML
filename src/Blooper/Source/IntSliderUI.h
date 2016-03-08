/*
  ==============================================================================

    IntSliderUI.h
    Created: 8 Mar 2016 3:46:34pm
    Author:  bkupe

  ==============================================================================
*/

#ifndef INTSLIDERUI_H_INCLUDED
#define INTSLIDERUI_H_INCLUDED

#include "ParameterUI.h"

class IntSliderUI : public ParameterUI, public Slider
{
public:
	IntSliderUI(Parameter * parameter);
	~IntSliderUI();

protected:
	void parameterValueChanged(Parameter *) override { repaint(); };

	JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(IntSliderUI)
};



#endif  // INTSLIDERUI_H_INCLUDED
