/*
  ==============================================================================

    FloatStepperUI.h
    Created: 28 Apr 2016 6:00:28pm
    Author:  bkupe

  ==============================================================================
*/

#ifndef FLOATSTEPPERUI_H_INCLUDED
#define FLOATSTEPPERUI_H_INCLUDED

#include "ParameterUI.h"

class FloatStepperUI : public ParameterUI
{

public:
	FloatStepperUI(Parameter * _parameter);
	virtual ~FloatStepperUI();

	TextButton plusBT;
	TextButton minusBT;
	Label valueLabel;

	int fixedDecimals;

	void resized() override;

protected:
	void valueChanged(const var &) override;

};


#endif  // FLOATSTEPPERUI_H_INCLUDED
