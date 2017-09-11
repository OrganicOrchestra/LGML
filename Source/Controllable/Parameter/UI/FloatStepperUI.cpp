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


#include "FloatStepperUI.h"
#include "../NumericParameter.h"

FloatStepperUI::FloatStepperUI(Parameter * _parameter) :ParameterUI(_parameter){

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

FloatStepperUI::~FloatStepperUI()
{

}

void FloatStepperUI::setScrollAllowed(bool b){
  slider->setScrollWheelEnabled(b);
}
void FloatStepperUI::resized()
{
  slider->setBounds(getLocalBounds());
}

void FloatStepperUI::valueChanged(const var & value)
{
  if ((float)value == slider->getValue()) return;

  slider->setValue(value,NotificationType::dontSendNotification);
}

void FloatStepperUI::sliderValueChanged(Slider * _slider)
{

  parameter->setValue(_slider->getValue());

}
void FloatStepperUI::rangeChanged(Parameter * p){
  FloatParameter * fp = parameter->getAs<FloatParameter>();
  jassert(fp);
  slider->setRange((float)fp->minimumValue, (float)fp->maximumValue,1);

}


void FloatStepperUI::componentParentHierarchyChanged (Component& c){
  if(&c==this){

    bool isInViewport = findParentComponentOfClass<Viewport>()!=nullptr;
    setScrollAllowed(!isInViewport);
  }
}
