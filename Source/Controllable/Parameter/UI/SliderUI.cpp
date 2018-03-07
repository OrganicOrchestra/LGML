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


#include "SliderUI.h"
#include "../../../UI/Style.h"
#include "../NumericParameter.h"
#include "../UndoableHelper.h"

//==============================================================================
template<class T>
SliderUI<T>::SliderUI (Parameter* parameter) :
    ParameterUI (parameter), fixedDecimals (2),
    defaultColor (0xff99ff66),
    scrollWheelAllowed(true)
{
    assignOnMousePosDirect = false;
    changeParamOnMouseUpOnly = false;
    orientation = HORIZONTAL;
    setSize (100, 10);
    scaleFactor = 1;
    jassert (parameter->getAs<MinMaxParameter>() || !parameter);
    if(parameter->isEditable){
        valueBox = new Label();
        valueBox->setEditable(true);
        valueBox->addListener(this);
        valueBox->setColour(TextEditor::backgroundColourId,Colours::black);
        valueBox->setColour(TextEditor::highlightColourId,Colours::grey);
        valueBox->setJustificationType(Justification::centred);
        valueBox->addMouseListener(this, false);
        addChildComponent(valueBox);
    }

}

template<class T>
SliderUI<T>::~SliderUI()
{

}


template<class T>
void SliderUI<T>::paint (Graphics& g)
{

    if (shouldBailOut())return;

    Rectangle<int> sliderBounds = getLocalBounds();

    const float corner = 2;
    auto bgColor =findColour (Slider::backgroundColourId);
    g.setColour (bgColor);
    g.fillRoundedRectangle (sliderBounds.toFloat(), corner);

//    g.setColour (findColour (Slider::backgroundColourId).withAlpha (1.f).brighter());
//    g.drawRoundedRectangle (sliderBounds.toFloat(), corner, 1);
    Colour baseColour = parameter->isEditable ? findColour (Slider::trackColourId) : findColour (Slider::trackColourId).withAlpha (0.3f);
    Colour c = (isMouseButtonDown() && changeParamOnMouseUpOnly) ? findColour (TextButton::buttonOnColourId) : baseColour;

    float normalizedValue = getParamNormalizedValue();
    g.setColour (c);


    sliderBounds.reduce (1, 1);
    float drawPos = 0;

    if (orientation == HORIZONTAL)
    {
        drawPos = changeParamOnMouseUpOnly ? getMouseXYRelative().x : normalizedValue * getWidth();
        g.fillRoundedRectangle (sliderBounds.removeFromLeft ((int)drawPos).toFloat(), corner);
    }
    else
    {
        drawPos = changeParamOnMouseUpOnly ? getMouseXYRelative().y : normalizedValue * getHeight();
        g.fillRoundedRectangle (sliderBounds.removeFromBottom ((int)drawPos).toFloat(), corner);
    }


    if ((showLabel || showValue))
    {
        //Colour textColor = normalizedValue > .5f?Colours::darkgrey : Colours::grey;

        g.setColour (Colours::grey);

        sliderBounds = getLocalBounds();
        Rectangle<int> destRect;

        if (orientation == VERTICAL)
        {
            //destRect = Rectangle<int>(0, 0, 100, 100);
            juce::AffineTransform at;
            at = at.rotated ((float) (-double_Pi / 2.0f)); // , sliderBounds.getCentreX(), sliderBounds.getCentreY());
            at = at.translated (0.f, (float)sliderBounds.getHeight());
            g.addTransform (at);
            destRect = Rectangle<int> (0, 0, sliderBounds.getHeight(), sliderBounds.getWidth()).withSizeKeepingCentre (sliderBounds.getHeight(), 12);
        }
        else
        {
            destRect = sliderBounds.withSizeKeepingCentre (sliderBounds.getWidth(), 12);
        }

        String text = "";

        if (showLabel)
        {
            text += customTextDisplayed.isNotEmpty()?customTextDisplayed:parameter.get()?parameter->niceName:"No Parameter";

            if (showValue) text += " : ";
        }

        if (showValue) text += String::formatted ("%." + String (fixedDecimals) + "f", parameter->floatValue());

        g.drawFittedText (text, destRect, Justification::centred, 1);
    }
}

template<class T>
void SliderUI<T>::resized() {

    if(valueBox){
        Rectangle<int> r = getLocalBounds();
        if(orientation==VERTICAL){
            r = r.transformedBy(AffineTransform::rotation(float_Pi/2,r.getCentreX(),r.getCentreY()));
        }
        valueBox->setBounds(r);
    }
}

template<class T>
void SliderUI<T>::mouseWheelMove (const MouseEvent& e, const MouseWheelDetails& wheel)
{
    Viewport *vp =findParentComponentOfClass<Viewport>();
    bool _scrollWheelAllowed = !vp  || !(vp->canScrollVertically() || vp->canScrollHorizontally());
        if (_scrollWheelAllowed&&scrollWheelAllowed)
        {
            // sometimes duplicate wheel events seem to be sent, so since we're going to
            // bump the value by a minimum of the interval, avoid doing this twice..
            if (e.eventTime != lastMouseWheelTime)
            {

                lastMouseWheelTime = e.eventTime;

                if (//maximum > minimum &&
                    ! e.mods.isAnyMouseButtonDown())
                {
                    float interval = 0.3;
                    if(e.mods.isShiftDown())interval/=10.0;
                    const float value = getParamNormalizedValue();
                    const float delta = (std::abs (wheel.deltaX) > std::abs (wheel.deltaY)
                                   ? -wheel.deltaX : wheel.deltaY)* (wheel.isReversed ? -1.0f : 1.0f);


                    if (delta != 0.0)
                    {
                        float newValue =jmin<float>(jmax<float>( value + interval*delta,0.f),1.f);
                        setParamNormalizedValue(newValue);
                    }
                }
            }
        }
        else{
            Component::mouseWheelMove (e, wheel);
        }
    }
    
template<class T>
void SliderUI<T>::mouseDown (const MouseEvent& e)
{
    ParameterUI::mouseDown (e);

    if (!parameter->isEditable) return;


    if (!e.mods.isLeftButtonDown()) return;

    initValue = getParamNormalizedValue();


    if (e.mods.isShiftDown())
    {
        parameter->resetValue();
    }

    if (e.mods.isCommandDown())
    {
        UndoableHelpers::setValueUndoable(parameter, parameter->floatValue()>0?0:(T)parameter->lastValue);
    }

    if (assignOnMousePosDirect)
    {
        setParamNormalizedValue ((float)getValueFromMouse());
    }
    else
    {
        repaint();
    }

}



template<class T>
void SliderUI<T>::mouseDrag (const MouseEvent& e)
{
    if (!parameter->isEditable) return;

    if (!e.mods.isLeftButtonDown()) return;
    if(e.mods.isCommandDown() ||  e.mods.isShiftDown()) return;

    if (changeParamOnMouseUpOnly) repaint();
    else
    {
        if (e.mods.isLeftButtonDown())
        {
            setMouseCursor (MouseCursor::NoCursor);
            if (assignOnMousePosDirect)
            {
                setParamNormalizedValue ((float)getValueFromMouse());
            }
            else
            {
                float diffValue = (float)getValueFromPosition ((e.getPosition() - e.getMouseDownPosition()));

                if (orientation == VERTICAL) diffValue -= 1;

                setParamNormalizedValue ((float)(initValue + diffValue * scaleFactor));
            }
        }
    }
}

template<class T>
void SliderUI<T>::mouseUp (const MouseEvent& me)
{
    ParameterUI::mouseUp(me);
    if (!parameter->isEditable) return;

    if (!me.mods.isLeftButtonDown()) return;
    if(me.mods.isCommandDown() ||  me.mods.isShiftDown()) return;

    BailOutChecker checker (this);

    if(me.getDistanceFromDragStart()==0 && !me.mods.isAnyModifierKeyDown()){
        if(valueBox && !valueBox->isVisible()){
            valueBox->setText(String((T)parameter->value), dontSendNotification);
            valueBox->setVisible(true);
            if(auto p =getParentComponent())p->addAndMakeVisible(valueBox);
            valueBox->showEditor();
            auto vb = getBoundsInParent();
            int w = vb.getWidth();
            if(w<vb.getHeight()){
                vb = vb.transformedBy(AffineTransform::rotation(float_Pi/2.0f,(float)vb.getCentreX(),(float)vb.getCentreY()));
            }
            vb.setHeight(jmax<int>(vb.getHeight(),valueBox->getFont().getHeight()+3));
            valueBox->setBounds(vb);

        }
    }
    if (me.getNumberOfClicks() >= 2 )
    {

        AlertWindow nameWindow ("Set a value", "Set a new value for this parameter", AlertWindow::AlertIconType::NoIcon, this);
        nameWindow.addTextEditor ("newValue", parameter->stringValue());

        if (parameter->isUserDefined)
        {
            nameWindow.addTextBlock ("min Value");
            nameWindow.addTextEditor ("minValue", parameter->getAs<MinMaxParameter>()->minimumValue);
            nameWindow.addTextBlock ("max Value");
            nameWindow.addTextEditor ("maxValue", parameter->getAs<MinMaxParameter>()->maximumValue);
        }

        nameWindow.addButton ("OK", 1, KeyPress (KeyPress::returnKey));
        nameWindow.addButton ("Cancel", 0, KeyPress (KeyPress::escapeKey));

        int result = nameWindow.runModalLoop();

        if (result)
        {
            if (parameter->isUserDefined)
            {
                float newMin = nameWindow.getTextEditorContents ("minValue").getFloatValue();
                float newMax = nameWindow.getTextEditorContents ("maxValue").getFloatValue();
                parameter->getAs<MinMaxParameter>()->setMinMax (newMin, newMax);
            }

            float newValue = nameWindow.getTextEditorContents ("newValue").getFloatValue();
            UndoableHelpers::setValueUndoable(parameter,newValue);

            if(valueBox)valueBox->hideEditor(true);


        }
    }

    if (changeParamOnMouseUpOnly )
    {
        setParamNormalizedValue ((float)getValueFromMouse());
    }
    else
    {
        repaint();
    }

    if (!checker.shouldBailOut())
    {
        setMouseCursor (MouseCursor::NormalCursor);
    }
}

template<class T>
T SliderUI<T>::getValueFromMouse()
{
    return getValueFromPosition (getMouseXYRelative());
}

template<class T>
T SliderUI<T>::getValueFromPosition (const Point<int>& pos)
{
    if (orientation == HORIZONTAL) return (pos.x * 1.0f / getWidth());
    else return 1 - (pos.y * 1.0f / getHeight());
}

template<class T>
void SliderUI<T>::setParamNormalizedValue (float value)
{
    auto np = parameter->getAs<NumericParameter<T> >();
    auto nv = (T)(jmap<T> (jmin<T> (jmax<T> (value, 0.0f), 1.0f), (T)np->minimumValue, (T)np->maximumValue));
    UndoableHelpers::setValueUndoable(parameter, nv);

}

template<class T>
float SliderUI<T>::getParamNormalizedValue()
{
    return parameter->getAs<NumericParameter<T> >()->getNormalizedValue();
}

template<class T>
void SliderUI<T>::valueChanged (const var&)
{
    if(valueBox)valueBox->hideEditor(true);
    repaint();
};
template<class T>
void SliderUI<T>::labelTextChanged (Label* labelThatHasChanged) {
    UndoableHelpers::setValueUndoable(parameter,(T)labelThatHasChanged->getText().getDoubleValue());
};
template<class T>
void SliderUI<T>::editorHidden (Label*, TextEditor&) {
    if(auto p = getParentComponent()){
        p->removeChildComponent(valueBox);
    }
    valueBox->setVisible(false);
    repaint();
};

template class SliderUI<int>;
template class SliderUI<double>;
