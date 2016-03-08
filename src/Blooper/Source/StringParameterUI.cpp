/*
  ==============================================================================

    StringParameterUI.cpp
    Created: 9 Mar 2016 12:29:57am
    Author:  bkupe

  ==============================================================================
*/


#include "StringParameterUI.h"

StringParameterUI::StringParameterUI(Parameter * p) :
	ParameterUI(p),
	stringParam((StringParameter *)p)
{
	addAndMakeVisible(valueLabel);

}

void StringParameterUI::resized()
{
	valueLabel.setBounds(getLocalBounds());
}


void StringParameterUI::parameterValueChanged(Parameter * p)
{
	ParameterUI::parameterValueChanged(p);
	valueLabel.setText(stringParam->value,NotificationType::dontSendNotification);
}

