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


#include "BoolToggleUI.h"
#include "../../../UI/Style.h"
#include "../BoolParameter.h"

//==============================================================================
BoolToggleUI::BoolToggleUI(Parameter * parameter) :
    ParameterUI(parameter)
{
    setSize(10,10);
}

BoolToggleUI::~BoolToggleUI()
{

}

void BoolToggleUI::paint(Graphics & g)
{

    // we are on component deletion
    if(shouldBailOut())return;

  Colour onColour = parameter->isEditable?findColour(TextButton::buttonOnColourId):findColour(TextButton::buttonColourId).withAlpha(0.f);

    bool valCheck = ((BoolParameter *)parameter.get())->invertVisuals ? !parameter->boolValue():parameter->boolValue();
    Colour c =  valCheck? onColour  : findColour(TextButton::buttonColourId);

    g.setGradientFill(ColourGradient(c.brighter(),(float)getLocalBounds().getCentreX(),(float)getLocalBounds().getCentreY(), c.darker(), 2.f,2.f,true));
    g.fillRoundedRectangle(getLocalBounds().toFloat(),2);

    g.setFont(10);
    g.setColour(Colours::white.darker(.1f));
  if(customTextDisplayed.isNotEmpty()){
    g.drawText(customTextDisplayed, getLocalBounds().reduced(2).toFloat(), Justification::centred);
  }
  else{
    g.drawText(parameter->niceName, getLocalBounds().reduced(2).toFloat(), Justification::centred);
  }
}

void BoolToggleUI::mouseDown(const MouseEvent &e)
{
	ParameterUI::mouseDown(e);

	if (!parameter->isEditable) return;
	if (e.mods.isLeftButtonDown())
	{
		parameter->setValue(!parameter->boolValue());
	}
}

void BoolToggleUI::mouseUp(const MouseEvent & e)
{
	ParameterUI::mouseUp(e);
	if (!parameter->isEditable) return;
    
	if (e.mods.isLeftButtonDown() && e.mods.isShiftDown()) parameter->setValue(!parameter->boolValue());
}

void BoolToggleUI::valueChanged(const var & )
{
    repaint();
}
