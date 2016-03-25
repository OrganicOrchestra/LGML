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
    displayText = true;
    displayBar = true;
	orientation = HORIZONTAL;
}

FloatSliderUI::~FloatSliderUI()
{
}


void FloatSliderUI::paint(Graphics & g)
{
	
	Colour c = (isMouseButtonDown() && changeParamOnMouseUpOnly) ? HIGHLIGHT_COLOR : PARAMETER_FRONT_COLOR;
	
	g.setColour(BG_COLOR);
	g.fillRoundedRectangle(getLocalBounds().toFloat(), 2);



    
    g.setColour(c);
    if(displayBar){
	float drawPos = 0; 
	if (orientation == HORIZONTAL)
	{
		drawPos = changeParamOnMouseUpOnly ? getMouseXYRelative().x : getParamNormalizedValue()*getWidth();
		g.fillRoundedRectangle(getLocalBounds().removeFromLeft(drawPos).toFloat(), 2);
	}
	else {
		drawPos = changeParamOnMouseUpOnly ? getMouseXYRelative().y : getParamNormalizedValue()*getHeight();
		g.fillRoundedRectangle(getLocalBounds().removeFromBottom(drawPos).toFloat(), 2);
	}
    }
    
    if(displayText){
        g.setColour(displayBar?Colours::darkgrey:c);
        Rectangle<int> destRect = getLocalBounds();
        if(orientation == VERTICAL){
            AffineTransform at;
            at = at.rotated(-double_Pi/2.0, getLocalBounds().getWidth()/2.0, getLocalBounds().getHeight()/2.0);
            g.addTransform(at);
            destRect.setBounds(0,0,getLocalBounds().getHeight(),getLocalBounds().getHeight());
        }
        g.drawText(String::formatted("%.2f",floatParam->value), destRect, Justification::centred);
    }
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
			if (orientation == VERTICAL) diffValue -= 1;

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
	else return 1-(pos.y*1.0 / getHeight());
}

void FloatSliderUI::setParamNormalizedValue(float value)
{
	floatParam->setNormalizedValue(value);
}

float FloatSliderUI::getParamNormalizedValue()
{
	return floatParam->getNormalizedValue();
}


