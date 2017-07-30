/*
 ==============================================================================

 RangeParameterUI.h
 Created: 30 Jul 2017 12:24:29pm
 Author:  Martin Hermant

 ==============================================================================
 */

#pragma once

#include "ParameterUI.h"

#include "RangeParameter.h"



class RangeParameterUI : public ParameterUI, private Slider::Listener{
public:
  RangeParameterUI(RangeParameter * p):ParameterUI(p),rangeP(p),slider(Slider::SliderStyle::TwoValueHorizontal,Slider::NoTextBox){
    slider.addListener(this);
    addAndMakeVisible(slider);
    slider.setRange(p->minimumValue, p->maximumValue);
    slider.setMinAndMaxValues(p->getRangeMin(),p->getRangeMax());
  }
  Slider slider;
  void valueChanged(const var & v) override{
    if(v.isArray() && v.size()>1){
      slider.setMinAndMaxValues(v[0],v[1]);
    }

  };

  void rangeChanged(Parameter * p)override{
    jassert(p==rangeP);
    slider.setRange(p->minimumValue, p->maximumValue);
  }
  void resized()override{
    slider.setBounds(getLocalBounds());
  }

  void sliderValueChanged (Slider* slider) override{
    rangeP->setValue(slider->getMinValue(),slider->getMaxValue());
  }

  RangeParameter * rangeP;

};
