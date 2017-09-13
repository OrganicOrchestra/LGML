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


#include "StepperUI.h"
#include "../NumericParameter.h"



template<class T>
StepperUI<T>::StepperUI(Parameter * _parameter) :ParameterUI(_parameter){

  slider = new BetterStepper(ParameterUI::getTooltip());
  MinMaxParameter * fp = parameter->getAs<MinMaxParameter>();
  jassert(fp);
  addAndMakeVisible(slider);
  slider->setRange(fp->minimumValue, fp->maximumValue,1);
  slider->setValue(parameter->floatValue(),dontSendNotification);
  slider->addListener(this);
  slider->setTextBoxIsEditable(_parameter->isEditable);
  slider->setEnabled( parameter->isEditable);
  addComponentListener(this);

}



template<class T>
StepperUI<T>::~StepperUI()
{

}
template<class T>
void StepperUI<T>::setScrollAllowed(bool b){
  slider->setScrollWheelEnabled(b);
}
template<class T>
void StepperUI<T>::resized()
{
  slider->setBounds(getLocalBounds());
}
template<class T>
void StepperUI<T>::valueChanged(const var & value)
{
  if ((float)value == slider->getValue()) return;

  slider->setValue(value,NotificationType::dontSendNotification);
}
template<class T>
void StepperUI<T>::sliderValueChanged(Slider * _slider)
{

  parameter->setValue(_slider->getValue());

}
template <class T>
void StepperUI<T>::rangeChanged(Parameter * p){
  auto * fp = parameter->getAs<NumericParameter<T>>();
  jassert(fp);
  slider->setRange((T)fp->minimumValue, (T)fp->maximumValue,1);

}

template<class T>
void StepperUI<T>::componentParentHierarchyChanged (Component& c){
  if(&c==this){

    bool isInViewport = findParentComponentOfClass<Viewport>()!=nullptr;
    setScrollAllowed(!isInViewport);
  }
}

template class StepperUI<double>;
template class StepperUI<int>;
