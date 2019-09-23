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

#if !ENGINE_HEADLESS

#include "SliderUI.h"
#include "../../../UI/Style.h"
#include "../NumericParameter.h"
#include "../UndoableHelper.h"

int maxDecimals = 2;
//==============================================================================
template<class T>
SliderUI<T>::SliderUI ( ParameterBase* parameter) :
    ParameterUI (parameter),
    scrollWheelAllowed(true)
{
//    showValue = false;
//    showLabel = false;
    if(auto fp = parameter->getAs<FloatParameter>()){
        if(fp->precisionMask>0){
            fixedDecimals = jmin(maxDecimals,(int)log10f(fp->precisionMask));
        }
        else{
            fixedDecimals = maxDecimals;
        }

    }
    else{
        fixedDecimals = 0;
    }

    assignOnMousePosDirect = false;
    changeParamOnMouseUpOnly = false;
    orientation = HORIZONTAL;
    setSize (100, 10);
    scaleFactor = 1;
    jassert (parameter->getAs<MinMaxParameter>() || !parameter);
    if(parameter->isEditable){
        valueBox = std::make_unique<Label>();
        valueBox->setEditable(true);
        valueBox->addListener(this);
        valueBox->setColour(TextEditor::backgroundColourId,Colours::black);
        valueBox->setColour(TextEditor::highlightColourId,Colours::grey);
        valueBox->setJustificationType(Justification::centred);
        valueBox->addMouseListener(this, false);
        valueBox->setPaintingIsUnclipped(true);
        addChildComponent(valueBox.get());
    }

    nameCachedLabel.setBorderSize({0,4,0,0});
    valueCachedLabel.setBorderSize({0,0,0,4});
    LGMLUIUtils::optionallySetBufferedToImage(&nameCachedLabel);
    LGMLUIUtils::optionallySetBufferedToImage(&valueCachedLabel);
    valueCachedLabel.setColour(Label::textColourId,Colours::white);
    nameCachedLabel.setColour(Label::textColourId,Colours::white);
    valueCachedLabel.setInterceptsMouseClicks(false,false);
    nameCachedLabel.setInterceptsMouseClicks(false,false);
    valueCachedLabel.setJustificationType(Justification::right);
    nameCachedLabel.setJustificationType(Justification::left);
    valueCachedLabel.setPaintingIsUnclipped(true);
    nameCachedLabel.setPaintingIsUnclipped(true);

    addChildComponent(nameCachedLabel);
    addChildComponent(valueCachedLabel);
    displayedTextChangedInternal();
    valueChanged(parameter->value);


}

template<class T>
SliderUI<T>::~SliderUI()
{

}

#define FILL_RECT_FUNC(a,r) fillRect(a)
//#define FILL_RECT_FUNC(a,r) fillRoundedRectangleRect(a.toFloat(),b)

template<class T>
void SliderUI<T>::paint (Graphics& g)
{

    if (shouldBailOut())return;
    if(showLabel!=nameCachedLabel.isVisible()){
        nameCachedLabel.setVisible(showLabel);
        if(showLabel)displayedTextChangedInternal();
    }
    if(showValue!=valueCachedLabel.isVisible()){
        valueCachedLabel.setVisible(showValue);
        if(showValue)valueChanged(parameter->value);
    }

    ParameterUI::paint(g);
    Rectangle<int> sliderBounds = getLocalBounds();

//    const float corner = 2;
    auto bgColor =findColour (Slider::backgroundColourId);
    g.setColour (bgColor);
    g.FILL_RECT_FUNC (sliderBounds, corner);


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
        g.FILL_RECT_FUNC (sliderBounds.removeFromLeft ((int)drawPos), corner);
    }
    else
    {
        drawPos = changeParamOnMouseUpOnly ? getMouseXYRelative().y : normalizedValue * getHeight();
        g.FILL_RECT_FUNC (sliderBounds.removeFromBottom ((int)drawPos), corner);
    }

}

template<class T>
void SliderUI<T>::displayedTextChangedInternal(){
    nameCachedLabel.setText(getDisplayedText(),dontSendNotification);
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
    if(showLabel || showValue){

        if(!showValue){
            nameCachedLabel.setBounds(getLocalBounds());
        }
        else{
            Rectangle<int> r = getLocalBounds();


//            int idealWidth = valueCachedLabel.getFont().getStringWidth(valueCachedLabel.getText()+" ");
//            idealValue = jmin(r.getWidth(),idealWidth);
            if(orientation==VERTICAL){// simulate horizontal but rotate it
                r.setWidth(r.getHeight());
                r.setHeight(getLocalBounds().getWidth());
                auto at = AffineTransform::rotation(-float_Pi/2.f,0,0).translated(0,r.getWidth());
                valueCachedLabel.setTransform(at);
                nameCachedLabel.setTransform(at);
            }
            Font bF = valueCachedLabel.getFont();
            auto bSize = valueCachedLabel.getBorderSize();
            int tFontH = jmin(jmax(r.getHeight()-(bSize.getTop()+bSize.getBottom()),5),13);
            if(bF.getHeight()!=tFontH){
                bF.setHeight(tFontH);
                valueCachedLabel.setFont(bF);
                nameCachedLabel.setFont(bF);
            }
            int valueSize =r.getWidth()/2;
            if(valueSize<30){valueSize=r.getWidth();}
            valueCachedLabel.setBounds(r.removeFromRight(valueSize));
            nameCachedLabel.setBounds(r);


        }

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
    if (!parameter){jassertfalse; return;}
    if (!parameter->isEditable) return;


    if (!e.mods.isLeftButtonDown()) return;

    initValue = getParamNormalizedValue();


    if (e.mods.isShiftDown())
    {
        UndoableHelpers::resetValueUndoable(parameter);
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
void SliderUI<T>::processUICommand(int i) {
    switch (i){
        case 1:
            UndoableHelpers::resetValueUndoable(parameter);
            break;
        case 2:
            UndoableHelpers::setValueUndoable(parameter, parameter->floatValue()>0?0:(T)parameter->lastValue);
            break;
        case 3: 
            launchEditWindow();
            break;
        default:
            break;

    }


};


template<class T>
void SliderUI<T>::launchEditWindow(){
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


template<class T>
const ParameterUI::UICommandType & SliderUI<T>::getUICommands( ) const {
    static UICommandType res;
    if(res.size()==0){
        res.set(1,juce::translate("reset value (Shift+click)"));
        res.set(2,juce::translate("toggle value (Meta+click)"));
        res.set(3,juce::translate("edit value... (double click)"));
    }
    return res;
};


template<class T>
void SliderUI<T>::mouseUp (const MouseEvent& me)
{
    ParameterUI::mouseUp(me);
    if (!parameter->isEditable) return;

    if (!me.mods.isLeftButtonDown()) return;
    if(me.mods.isCommandDown() ||  me.mods.isShiftDown()) return;

    BailOutChecker checker (this);

    if(!me.mouseWasDraggedSinceMouseDown() && !me.mods.isAnyModifierKeyDown()){
        if(valueBox && !valueBox->isVisible()){
            valueBox->setText(String((T)parameter->value), dontSendNotification);
            valueBox->setVisible(true);
            if(auto p =getParentComponent())p->addAndMakeVisible(valueBox.get());
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

String getFormatedValueText(float f,int precision){
    return String::formatted ("%." + String (precision) + "f", f);
}
template<class T>
void SliderUI<T>::valueChanged (const var& v)
{
    if(valueBox)valueBox->hideEditor(true);
    if(showValue){
        valueCachedLabel.setText(getFormatedValueText(v, fixedDecimals),dontSendNotification);
    }
    repaint();
};
template<class T>
void SliderUI<T>::labelTextChanged (Label* labelThatHasChanged) {
    UndoableHelpers::setValueUndoable(parameter,(T)labelThatHasChanged->getText().getDoubleValue());
};
template<class T>
void SliderUI<T>::editorHidden (Label*, TextEditor&) {
    if(auto p = getParentComponent()){
        p->removeChildComponent(valueBox.get());
    }
    valueBox->setVisible(false);
    repaint();
};

template class SliderUI<int>;
template class SliderUI<floatParamType>;

#endif
