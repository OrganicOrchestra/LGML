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

    addChildComponent(nameLabel);
    setNameLabelVisible(true);
    addAndMakeVisible(valueLabel);

    nameLabel.setJustificationType(Justification::topLeft);
    nameLabel.setText(stringParam->niceName, NotificationType::dontSendNotification);
    nameLabel.setColour(Label::ColourIds::textColourId, TEXTNAME_COLOR);

    valueLabel.setJustificationType(Justification::topLeft);
    valueLabel.setText(stringParam->value,NotificationType::dontSendNotification);
    valueLabel.setColour(Label::ColourIds::textColourId, TEXT_COLOR);
    valueLabel.setEditable(true);
    valueLabel.addListener(this);

    nameLabel.setTooltip(p->description);

    setSize(200, 20);//default size
}

void StringParameterUI::setNameLabelVisible(bool visible)
{
//    if (nameLabelIsVisible == visible) return;
    nameLabelIsVisible = visible;
    nameLabel.setVisible(visible);
}

void StringParameterUI::resized()
{
    Rectangle<int> r = getLocalBounds();
    if(nameLabelIsVisible) nameLabel.setBounds(r.removeFromLeft(100));
    valueLabel.setBounds(r);
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
