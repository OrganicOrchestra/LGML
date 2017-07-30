/*
  ==============================================================================

    RangeParameter.cpp
    Created: 30 Jul 2017 12:17:48pm
    Author:  Martin Hermant

  ==============================================================================
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

void RangeParameter::setValue(Point<float> _value)
{
  setValue(_value.x, _value.y);
}

void RangeParameter::setValue(float _x, float _y)
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

Point<float> RangeParameter::getRangeMinMax() {
  return Point<float>(getRangeMin(), getRangeMax());
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
