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
	assignOnMousePosDirect = true;
	changeParamOnMouseUpOnly = false;
	orientation = HORIZONTAL;
}

FloatSliderUI::~FloatSliderUI()
{
}


void FloatSliderUI::paint(Graphics & g)
{
	
	Colour c = isMouseButtonDown() && changeParamOnMouseUpOnly ? HIGHLIGHT_COLOR : PARAMETER_FRONT_COLOR;
	
	g.setColour(BG_COLOR);
	g.fillRoundedRectangle(getLocalBounds().toFloat(), 2);

	g.setColour(c);
	float drawPos = changeParamOnMouseUpOnly ? getMouseXYRelative().x : getParamNormalizedValue()*getWidth();
	g.fillRoundedRectangle(getLocalBounds().removeFromLeft(drawPos).toFloat(), 2);
}

void FloatSliderUI::mouseDown(const MouseEvent & e)
{
	if (assignOnMousePosDirect && !e.mods.isRightButtonDown())
	{
		setParamNormalizedValue(getValueFromMouse());
	}
	else
	{
		repaint();
	}

	initValue = getParamNormalizedValue();
	setMouseCursor(MouseCursor::NoCursor);
	
}

void FloatSliderUI::mouseDrag(const MouseEvent & e)
{
	if(changeParamOnMouseUpOnly) repaint();
	else
	{
		if (assignOnMousePosDirect && !e.mods.isRightButtonDown()) setParamNormalizedValue(getValueFromMouse());
		else
		{
			float diffValue = getValueFromPosition(e.getPosition()-e.getMouseDownPosition());
			setParamNormalizedValue(initValue + diffValue);
		}
	}
}

void FloatSliderUI::mouseUp(const MouseEvent & e)
{
	if (changeParamOnMouseUpOnly)
	{
		setParamNormalizedValue(getValueFromMouse());
	}
	else
	{
		repaint();
	}

	setMouseCursor(MouseCursor::NormalCursor);
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

void FloatSliderUI::setParamNormalizedValue(float value)
{
	floatParam->setNormalizedValue(value);
}

float FloatSliderUI::getParamNormalizedValue()
{
	return floatParam->getNormalizedValue();
}


