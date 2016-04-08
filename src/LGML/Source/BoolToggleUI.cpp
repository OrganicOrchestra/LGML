/*
  ==============================================================================

    BoolToggleUI.cpp
    Created: 8 Mar 2016 3:47:01pm
    Author:  bkupe

  ==============================================================================
*/

#include "BoolToggleUI.h"
#include "Style.h"

//==============================================================================
BoolToggleUI::BoolToggleUI(Parameter * parameter) :
    ParameterUI(parameter),
    boolParam((BoolParameter *)parameter)
{
    setSize(10,10);
}

BoolToggleUI::~BoolToggleUI()
{

}

void BoolToggleUI::paint(Graphics & g)
{
    Colour c = boolParam->value ? HIGHLIGHT_COLOR : NORMAL_COLOR;
    g.setGradientFill(ColourGradient(c.brighter(),(float)getLocalBounds().getCentreX(),(float)getLocalBounds().getCentreY(), c.darker(), 2.f,2.f,true));
    g.fillRoundedRectangle(getLocalBounds().toFloat(),2);
}

void BoolToggleUI::mouseDown(const MouseEvent &)
{
    boolParam->setValue(!boolParam->value);
}

void BoolToggleUI::mouseUp(const MouseEvent & e)
{
    if (e.mods.isRightButtonDown()) boolParam->setValue(!boolParam->value);
}

void BoolToggleUI::parameterValueChanged(Parameter *)
{
    repaint();
}
