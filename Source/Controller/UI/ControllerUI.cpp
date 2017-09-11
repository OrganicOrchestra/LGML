/* Copyright © Organic Orchestra, 2017
*
* This file is part of LGML.  LGML is a software to manipulate sound in realtime
*
* This program is free software; you can redistribute it and/or modify
* it under the terms of the GNU General Public License as published by
* the Free Software Foundation (version 3 of the License).
*
* This program is distributed in the hope that it will be useful,
* but WITHOUT ANY WARRANTY; without even the implied warranty of
* MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
*
*/


#include "ControllerUI.h"
#include "ControllerEditor.h"
#include "../../UI/Style.h"
#include "../../Controllable/Parameter/UI/TriggerBlinkUI.h"
#include "../../Controllable/Parameter/UI/StringParameterUI.h"
#include "../../Controllable/Parameter/UI/ParameterUIFactory.h"


ControllerUI::ControllerUI(Controller * controller) :
	InspectableComponent(controller, "controller"),
    controller(controller)
{
  canInspectChildContainersBeyondRecursion = true;
  {
    MessageManagerLock ml;
	addMouseListener(this, true);
  }
	nameTF = new StringParameterUI(controller->nameParam);
  nameTF->setBackGroundIsTransparent(true);
	addAndMakeVisible(nameTF);

  enabledBT = ParameterUIFactory::createDefaultUI(controller->enabledParam);
	addAndMakeVisible(enabledBT);

	Image removeImage = ImageCache::getFromMemory(BinaryData::removeBT_png, BinaryData::removeBT_pngSize);

	removeBT.setImages(false, true, true, removeImage,
		0.7f, Colours::transparentBlack,
		removeImage, 1.0f, Colours::transparentBlack,
		removeImage, 1.0f, Colours::white.withAlpha(.7f),
		0.5f);
	removeBT.addListener(this);
	addAndMakeVisible(removeBT);


  activityBlink = new TriggerBlinkUI(controller->activityTrigger);
	activityBlink->showLabel = false;
	addAndMakeVisible(activityBlink);
  
}

ControllerUI::~ControllerUI()
{

}

void ControllerUI::paint(Graphics & g)
{
    g.setColour(PANEL_COLOR);
    g.fillRoundedRectangle(getLocalBounds().toFloat(), 4);

}

void ControllerUI::resized()
{
	Rectangle<int> r = getLocalBounds().reduced(2);
	r.removeFromRight(15);
	removeBT.setBounds(r.removeFromRight(20));
	r.removeFromRight(2);
	activityBlink->setBounds(r.removeFromRight(r.getHeight()).reduced(2));
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
		controller->parentContainer->removeChildControllableContainer(controller);
		return true;
	}

	return false;
}

InspectorEditor * ControllerUI::createEditor()
{
  return controller->createEditor();
}
