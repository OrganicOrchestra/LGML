/*
  ==============================================================================

    StringParameterUI.cpp
    Created: 9 Mar 2016 12:29:57am
    Author:  bkupe

  ==============================================================================
*/


#include "StringParameterUI.h"
#include "Style.h"

StringParameterUI::StringParameterUI(Parameter * p) :
	ParameterUI(p),
	stringParam((StringParameter *)p)
{
	addAndMakeVisible(valueLabel);
	
	valueLabel.setJustificationType(Justification::topLeft);
	valueLabel.setText(stringParam->value,NotificationType::dontSendNotification);
	valueLabel.setColour(Label::ColourIds::textColourId, TEXT_COLOR);
	valueLabel.setEditable(true);
	valueLabel.addListener(this);
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

void StringParameterUI::labelTextChanged(Label * labelThatHasChanged)
{
	stringParam->setValue(valueLabel.getText());
}

