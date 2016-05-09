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
    ParameterUI(p)
{

    addChildComponent(nameLabel);
    setNameLabelVisible(true);
    addAndMakeVisible(valueLabel);

    nameLabel.setJustificationType(Justification::topLeft);
    nameLabel.setText(parameter->niceName, NotificationType::dontSendNotification);
    nameLabel.setColour(Label::ColourIds::textColourId, TEXTNAME_COLOR);

    valueLabel.setJustificationType(Justification::topLeft);
    valueLabel.setText(parameter->value,NotificationType::dontSendNotification);
    valueLabel.setColour(Label::ColourIds::textColourId, TEXT_COLOR);
    valueLabel.setEditable(false,true);
    valueLabel.addListener(this);

    //valueLabel.setColour(valueLabel.textWhenEditingColourId, TEXTNAME_COLOR);
    valueLabel.setColour(valueLabel.backgroundWhenEditingColourId, Colours::white);
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


void StringParameterUI::valueChanged(const var & v)
{
    valueLabel.setText(v,NotificationType::dontSendNotification);
}

void StringParameterUI::labelTextChanged(Label *)
{
    parameter->setValue(valueLabel.getText());
}
