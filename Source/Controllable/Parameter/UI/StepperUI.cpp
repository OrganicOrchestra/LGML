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

#include "StepperUI.h"
#include "../NumericParameter.h"
#include "../UndoableHelper.h"


template<class T>
StepperUI<T>::StepperUI ( ParameterBase* _parameter) : ParameterUI (_parameter)
{

    slider = new BetterStepper (this);
    MinMaxParameter* fp = parameter->getAs<MinMaxParameter>();
    jassert (fp);
    addAndMakeVisible (slider);
    stepInterval =(T)(.1)==0?1:0.01;
    slider->setRange (fp->minimumValue, fp->maximumValue,(double) stepInterval);
    slider->setValue (parameter->floatValue(), dontSendNotification);
    slider->addListener (this);
    
    slider->setEditable(_parameter->isEditable);
    addMouseListener(this, true);
    

}



template<class T>
StepperUI<T>::~StepperUI()
{
    removeMouseListener(this);
}
template<class T>
void StepperUI<T>::mouseDown(const MouseEvent & e){
    ParameterUI::mouseDown(e);
}

template<class T>
void StepperUI<T>::resized()
{
    slider->setBounds (getLocalBounds());
}

template<class T>
void StepperUI<T>::setStepInterval(T i){
    stepInterval = i;
    rangeChanged(parameter);
}

template<class T>
void StepperUI<T>::valueChanged (const var& value)
{
    if ((float)value == slider->getValue()) return;

    slider->setValue (value, NotificationType::dontSendNotification);
}
template<class T>
void StepperUI<T>::sliderValueChanged (Slider* _slider)
{
    UndoableHelpers::setValueUndoable(parameter, _slider->getValue());

}
template <class T>
void StepperUI<T>::rangeChanged ( ParameterBase* p)
{
    auto* fp = parameter->getAs<NumericParameter<T>>();
    jassert (fp);
    slider->setRange ((T)fp->minimumValue, (T)fp->maximumValue, stepInterval);

}


template class StepperUI<double>;
template class StepperUI<int>;

#endif
