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
