/*
  ==============================================================================

    BoolToggleUI.cpp
    Created: 8 Mar 2016 3:47:01pm
    Author:  bkupe

  ==============================================================================
*/

#include "BoolToggleUI.h"
#include "Style.h"
#include "BoolParameter.h"

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

	Colour onColour = parameter->isEditable?HIGHLIGHT_COLOR:FEEDBACK_COLOR;

    bool valCheck = ((BoolParameter *)parameter.get())->invertVisuals ? !parameter->boolValue():parameter->boolValue();
    Colour c =  valCheck? onColour  : NORMAL_COLOR;

    g.setGradientFill(ColourGradient(c.brighter(),(float)getLocalBounds().getCentreX(),(float)getLocalBounds().getCentreY(), c.darker(), 2.f,2.f,true));
    g.fillRoundedRectangle(getLocalBounds().toFloat(),2);

    g.setFont(10);
    g.setColour(Colours::white.darker(.1f));
    g.drawText(parameter->niceName, getLocalBounds().reduced(2).toFloat(), Justification::centred);
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
