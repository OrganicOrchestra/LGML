/*
  ==============================================================================

    FloatSliderUI.cpp
    Created: 8 Mar 2016 3:46:27pm
    Author:  bkupe

  ==============================================================================
*/

#include "FloatSliderUI.h"
#include "Style.h"

//==============================================================================
FloatSliderUI::FloatSliderUI(Parameter * parameter) :
	ParameterUI(parameter), floatParam((FloatParameter *)parameter)
{
	changeParamOnMouseUpOnly = false;
	assignOnMousePosDirect = true;
	orientation = HORIZONTAL;
}

FloatSliderUI::~FloatSliderUI()
{
}


void FloatSliderUI::paint(Graphics & g)
{
	
	Colour c = (isMouseOverOrDragging() ? HIGHLIGHT_COLOR : NORMAL_COLOR).withAlpha(floatParam->enabled ? 1 : .3f);
	
	g.setGradientFill(ColourGradient(NORMAL_COLOR, getLocalBounds().getCentreX(), getLocalBounds().getCentreY(), NORMAL_COLOR.darker(), 2, 2, true));
	g.fillRoundedRectangle(getLocalBounds().toFloat(), 2);

	g.setColour(c);
	g.fillRoundedRectangle(getLocalBounds().removeFromLeft(floatParam->value*getWidth()).toFloat(), 2);
}

void FloatSliderUI::mouseDown(const MouseEvent & e)
{
	if (assignOnMousePosDirect)
	{
		DBG(getValueFromMouse());
		floatParam->setNormalizedValue(getValueFromMouse());
	}
	else
	{
		repaint();
	}

	initValue = floatParam->getNormalizedValue();
	//setMouseCursor(MouseCursor::NoCursor);
	
}

void FloatSliderUI::mouseDrag(const MouseEvent & e)
{
	if(changeParamOnMouseUpOnly) repaint();
	else
	{
		if (assignOnMousePosDirect) floatParam->setNormalizedValue(getValueFromMouse());
		else
		{
			float diffValue = getValueFromPosition(e.getPosition()-e.getMouseDownPosition());
			floatParam->setNormalizedValue(initValue + diffValue);
		}
	}
}

void FloatSliderUI::mouseUp(const MouseEvent & e)
{
	if (changeParamOnMouseUpOnly)
	{
		floatParam->setNormalizedValue(getValueFromMouse());
	}
	else
	{
		repaint();
	}

	//setMouseCursor(MouseCursor::NormalCursor);
}

float FloatSliderUI::getValueFromMouse()
{
	return getValueFromPosition(getMouseXYRelative());
}

float FloatSliderUI::getValueFromPosition(const Point<int> &pos)
{
	if (orientation == HORIZONTAL) return (pos.x*1.0 / getWidth());
	else return (pos.y*1.0 / getHeight());
}


