/*
  ==============================================================================

    FloatStepperUI.cpp
    Created: 28 Apr 2016 6:00:28pm
    Author:  bkupe

  ==============================================================================
*/

#include "FloatStepperUI.h"

FloatStepperUI::FloatStepperUI(Parameter * _parameter) :
	ParameterUI(_parameter), fixedDecimals(3)
{

}

FloatStepperUI::~FloatStepperUI()
{

}

void FloatStepperUI::resized()
{
	int btWidth = 15;
	Rectangle<int> r = getLocalBounds();
	valueLabel.setBounds(r.removeFromLeft(getWidth() - btWidth));
	plusBT.setBounds(r.removeFromBottom(r.getHeight() / 2));
	minusBT.setBounds(r);
}

void FloatStepperUI::valueChanged(const var & value)
{
	String l = String::formatted("%." + String(fixedDecimals) + "f", parameter->floatValue());
	if (showLabel) l = parameter->niceName + " " + l;
	valueLabel.setText(l, NotificationType::dontSendNotification);
}
