/*
  ==============================================================================

    FloatSliderUI.h
    Created: 8 Mar 2016 3:46:27pm
    Author:  bkupe

  ==============================================================================
*/

#ifndef FLOATSLIDERUI_H_INCLUDED
#define FLOATSLIDERUI_H_INCLUDED

#include "ParameterUI.h"


class FloatSliderUI    : public ParameterUI, public Component
{
public:
    FloatSliderUI(Parameter * parameter);
    ~FloatSliderUI();

protected:
	void parameterValueChanged(Parameter *) override { repaint(); };

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (FloatSliderUI)
};


#endif  // FLOATSLIDERUI_H_INCLUDED
