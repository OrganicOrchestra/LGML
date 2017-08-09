/*
 ==============================================================================

 FloatStepperUI.cpp
 Created: 28 Apr 2016 6:00:28pm
 Author:  bkupe

 ==============================================================================
 */

#include "FloatStepperUI.h"
#include "LGMLComponentHelpers.h"


FloatStepperUI::FloatStepperUI(Parameter * _parameter) :ParameterUI(_parameter){

  slider = new BetterStepper(ParameterUI::getTooltip());

  addAndMakeVisible(slider);
  slider->setRange(parameter->minimumValue, parameter->maximumValue,1);
  slider->setValue(parameter->floatValue(),dontSendNotification);
  slider->addListener(this);
  slider->setTextBoxIsEditable(_parameter->isEditable);
  slider->setEnabled( ! parameter->isControllableFeedbackOnly);
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
  slider->setRange((float)p->minimumValue, (float)p->maximumValue,1);

}


void FloatStepperUI::componentParentHierarchyChanged (Component& c){
  if(&c==this){
    
    bool isInViewport = getParentOfType<Viewport>(this)!=nullptr;
    setScrollAllowed(!isInViewport);
  }
}
