/*
  ==============================================================================

    ControlVariableUI.cpp
    Created: 10 May 2016 3:02:05pm
    Author:  bkupe

  ==============================================================================
*/

#include "ControlVariableUI.h"

ControlVariableUI::ControlVariableUI(ControlVariable * _variable) :
	variable(_variable),
	pNameLabel("variableName", _variable->parameter->niceName)
{

	addAndMakeVisible(&pNameLabel);
	pNameLabel.setColour(pNameLabel.textColourId, TEXTNAME_COLOR);
	pNameLabel.setColour(pNameLabel.backgroundWhenEditingColourId, Colours::white);
	pNameLabel.addListener(this);

	pui = (ParameterUI *)variable->parameter->createDefaultUI();
	pui->showLabel = false;
	addAndMakeVisible(pui);


	Image removeImage = ImageCache::getFromMemory(BinaryData::removeBT_png, BinaryData::removeBT_pngSize);

	removeBT.setImages(false, true, true, removeImage,
		0.7f, Colours::transparentBlack,
		removeImage, 1.0f, Colours::transparentBlack,
		removeImage, 1.0f, Colours::pink.withAlpha(0.8f),
		0.5f);
	removeBT.addListener(this);
	addAndMakeVisible(&removeBT);
}

ControlVariableUI::~ControlVariableUI()
{
}

void ControlVariableUI::setNameIsEditable(bool value)
{
	pNameLabel.setEditable(false, value);

}

void ControlVariableUI::paint(Graphics & g)
{
	g.setColour(PANEL_COLOR.darker());
	g.fillRoundedRectangle(getLocalBounds().toFloat(), 2);
}

void ControlVariableUI::resized()
{
	Rectangle<int> r = getLocalBounds();
	removeBT.setBounds(r.removeFromRight(getHeight()).reduced(2));
	r.removeFromRight(5);
	pNameLabel.setBounds(r.removeFromLeft(100));
	pui->setBounds(r.reduced(2));
}

void ControlVariableUI::buttonClicked(Button * b)
{
	if (b == &removeBT)
	{
		variable->remove();
	}
}

void ControlVariableUI::labelTextChanged(Label * b)
{
	if (b == &pNameLabel)
	{
		String t = b->getText();
		if (t[0] == '/')
		{
			t = t.substring(1);
			pNameLabel.setText(t, NotificationType::dontSendNotification);
		}
		variable->parameter->setNiceName(t);
	}
}
