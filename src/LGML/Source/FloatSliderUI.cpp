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
ParameterUI(parameter)
{
    assignOnMousePosDirect = true;
    changeParamOnMouseUpOnly = false;
    displayText = true;
    displayBar = true;
    orientation = HORIZONTAL;
    setSize(100,10);
}

FloatSliderUI::~FloatSliderUI()
{
}


void FloatSliderUI::paint(Graphics & g)
{

    if(shouldBailOut())return;

    Colour c = (isMouseButtonDown() && changeParamOnMouseUpOnly) ? HIGHLIGHT_COLOR : PARAMETER_FRONT_COLOR;

    Rectangle<int> sliderBounds = getLocalBounds();


    float normalizedValue = getParamNormalizedValue();
    g.setColour(BG_COLOR);
    g.fillRoundedRectangle(sliderBounds.toFloat(), 2);

    g.setColour(c);
    if(displayBar){
        float drawPos = 0;
        if (orientation == HORIZONTAL)
        {
            drawPos = changeParamOnMouseUpOnly ? getMouseXYRelative().x : normalizedValue*getWidth();
            g.fillRoundedRectangle(sliderBounds.removeFromLeft((int)drawPos).toFloat(), 2.f);
        }
        else {
            drawPos = changeParamOnMouseUpOnly ? getMouseXYRelative().y : normalizedValue*getHeight();
            g.fillRoundedRectangle(sliderBounds.removeFromBottom((int)drawPos).toFloat(), 2.f);
        }
    }

    sliderBounds = getLocalBounds();

    if(displayText){
        Colour textColor = normalizedValue > .5f?Colours::darkgrey : Colours::lightgrey;
        g.setColour(displayBar?textColor:c);
        Rectangle<int> destRect = sliderBounds;

        if(orientation == VERTICAL){
            AffineTransform at;
            at = at.rotated((float)(-double_Pi/2.0f), sliderBounds.getWidth()/2.0f, sliderBounds.getHeight()/2.0f);
            g.addTransform(at);
            destRect.setBounds(-sliderBounds.getCentreY(),0, sliderBounds.getHeight(), sliderBounds.getHeight());
        }

        g.drawText(parameter->niceName+" : "+String::formatted("%.2f", parameter->floatValue()), destRect.reduced(2,0), Justification::left);
    }
}

void FloatSliderUI::mouseDown(const MouseEvent & e)
{
    BailOutChecker checker (this);
    if (assignOnMousePosDirect && !e.mods.isRightButtonDown())
    {
        setParamNormalizedValue(getValueFromMouse());
    }
    else
    {
        repaint();
    }

    // a component can delete itself ( by trigger the whole rebuilding of Inspector for example)
    // thank JUCE for having the perfect class for it
    if(!checker.shouldBailOut()){
        initValue = getParamNormalizedValue();
        setMouseCursor(MouseCursor::NoCursor);
    }

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

void FloatSliderUI::mouseUp(const MouseEvent &)
{
    BailOutChecker checker (this);
    if (changeParamOnMouseUpOnly)
    {
        setParamNormalizedValue(getValueFromMouse());
    }
    else
    {
        repaint();
    }

    if(!checker.shouldBailOut()){
        setMouseCursor(MouseCursor::NormalCursor);
    }
}

float FloatSliderUI::getValueFromMouse()
{
    return getValueFromPosition(getMouseXYRelative());
}

float FloatSliderUI::getValueFromPosition(const Point<int> &pos)
{
    if (orientation == HORIZONTAL) return (pos.x*1.0f / getWidth());
    else return 1-(pos.y*1.0f/ getHeight());
}

void FloatSliderUI::setParamNormalizedValue(float value)
{
    parameter->setNormalizedValue(value);
}

float FloatSliderUI::getParamNormalizedValue()
{
    return parameter->getNormalizedValue();
}

void FloatSliderUI::valueChanged(const var &) {
    repaint();
};
