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


#include "RangeParameter.h"



RangeParameter::RangeParameter(const String & niceName, const String & description,float _min,float _max,float init_min,float init_max, bool enabled) :
Parameter(RANGE, niceName, description,0,0,1,enabled)
{
//  hideInEditor = true;
  value .append(0);
  value .append(1);
  setRange(_min, _max);
  setValue(init_min, init_max);

}

void RangeParameter::setValue(Point<double> _value)
{
  setValue(_value.x, _value.y);
}

void RangeParameter::setValue(double _x, double _y)
{
  var d;
  d.append(_x);
  d.append(_y);
  Parameter::setValue(d);
}

void RangeParameter::setValueInternal(var & _value)
{

  if (!_value.isArray()){jassertfalse; return;}
  value[0] = jlimit<float>(minimumValue, maximumValue, _value[0]);
  value[1] = jlimit<float>(minimumValue, maximumValue,_value[1]);

}

Point<double> RangeParameter::getRangeMinMax() {
  return Point<double>(getRangeMin(), getRangeMax());
}
float RangeParameter::getRangeMin(){
  return value[0];
}
float RangeParameter::getRangeMax(){
  return value[1];
}
bool RangeParameter::checkValueIsTheSame(var newValue, var oldValue)
{
  if (!(newValue.isArray() && oldValue.isArray())) return false;

  return newValue[0] == oldValue[0] && newValue[1] == oldValue[1];
}
