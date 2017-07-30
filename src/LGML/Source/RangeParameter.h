/*
  ==============================================================================

    RangeParameter.h
    Created: 30 Jul 2017 12:17:48pm
    Author:  Martin Hermant

  ==============================================================================
*/

#pragma once


#include "Parameter.h"

class RangeParameter : public Parameter
{
public:
  RangeParameter(const String &niceName, const String &description,float min=0.0f,float max=1.0f,float init_min=0.0f,float init_max=1.0f, bool enabled = true);
  ~RangeParameter() {}



  
  void setValue(Point<float> value);
  void setValue(float x, float y);
  void setValueInternal(var & _value) override;

  Point<float> getRangeMinMax();
  float getRangeMin();
  float getRangeMax();

  bool checkValueIsTheSame(var newValue, var oldValue) override;


  JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(RangeParameter)
};
