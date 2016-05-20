/*
  ==============================================================================

    ControllerUI.cpp
    Created: 8 Mar 2016 10:46:01pm
    Author:  bkupe

  ==============================================================================
*/

#include "ControllerUI.h"
#include "ControllerEditor.h"
#include "Style.h"

ControllerUI::ControllerUI(Controller * controller) :
    controller(controller)
{
	addMouseListener(this, true);

	nameTF = controller->nameParam->createStringParameterUI();
	nameTF->setNameLabelVisible(false);
	addAndMakeVisible(nameTF);

	enabledBT = controller->enabledParam->createToggle();
	addAndMakeVisible(enabledBT);

	Image removeImage = ImageCache::getFromMemory(BinaryData::removeBT_png, BinaryData::removeBT_pngSize);

	removeBT.setImages(false, true, true, removeImage,
		0.7f, Colours::transparentBlack,
		removeImage, 1.0f, Colours::transparentBlack,
		removeImage, 1.0f, Colours::white.withAlpha(.7f),
		0.5f);
	removeBT.addListener(this);
	addAndMakeVisible(removeBT);
}

ControllerUI::~ControllerUI()
{

}

void ControllerUI::paint(Graphics & g)
{
    g.setColour(PANEL_COLOR);
    g.fillRoundedRectangle(getLocalBounds().toFloat(), 4);
	g.setColour(isSelected ? HIGHLIGHT_COLOR: PANEL_COLOR.darker());
    g.drawRoundedRectangle(getLocalBounds().toFloat(), 4, 2);
}

void ControllerUI::resized()
{
	Rectangle<int> r = getLocalBounds().reduced(2);
	r.removeFromRight(15);
	removeBT.setBounds(r.removeFromRight(20));
	enabledBT->setBounds(r.removeFromLeft(r.getHeight()));
	r.removeFromLeft(5);
	nameTF->setBounds(r);
}

void ControllerUI::mouseDown(const MouseEvent &)
{
	selectThis();
}

void ControllerUI::buttonClicked(Button * b)
{
	if (b == &removeBT)
	{
		controller->remove();
	}
}

bool ControllerUI::keyPressed(const KeyPress & key)
{
	if (!isSelected) return false;

	if (key.getKeyCode() == KeyPress::deleteKey || key.getKeyCode() == KeyPress::backspaceKey)
	{
		controller->remove();
		return true;
	}

	return false;
}

InspectorEditor * ControllerUI::getEditor()
{
	return new ControllerEditor(this);
}
