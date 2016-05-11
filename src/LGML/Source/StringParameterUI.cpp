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
    nameLabel.setText(prefix+parameter->niceName+suffix, NotificationType::dontSendNotification);
    nameLabel.setColour(Label::ColourIds::textColourId, TEXTNAME_COLOR);

    valueLabel.setJustificationType(Justification::topLeft);
    valueLabel.setText(parameter->value,NotificationType::dontSendNotification);
    valueLabel.setColour(Label::ColourIds::textColourId, TEXT_COLOR);
	valueLabel.setEditable(false,parameter->isEditable); 
	valueLabel.addListener(this);

    //valueLabel.setColour(valueLabel.textWhenEditingColourId, TEXTNAME_COLOR);
    valueLabel.setColour(valueLabel.backgroundWhenEditingColourId, Colours::white);
    nameLabel.setTooltip(p->description);

	

    setSize(200, 20);//default size
}

void StringParameterUI::setPrefix(const String & _prefix)
{
	if (prefix == _prefix) return;
	prefix = _prefix;
	valueChanged(parameter->stringValue());
}

void StringParameterUI::setSuffix(const String & _suffix)
{
	if (suffix == _suffix) return;
	suffix = _suffix;
	valueChanged(parameter->stringValue());
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
    if(nameLabelIsVisible) nameLabel.setBounds(r.removeFromLeft((int)(getWidth()*.4f)));
    valueLabel.setBounds(r);
}


void StringParameterUI::valueChanged(const var & v)
{
    valueLabel.setText(prefix+v.toString()+suffix,NotificationType::dontSendNotification);
}

void StringParameterUI::labelTextChanged(Label *)
{
    parameter->setValue(valueLabel.getText().removeCharacters(prefix).removeCharacters(suffix));
}
