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


#include "Point2DParameter.h"

#include "ParameterFactory.h"

REGISTER_PARAM_TYPE_TEMPLATED(Point2DParameter,int)
REGISTER_PARAM_TYPE_TEMPLATED(Point2DParameter,float)



template<typename T>
Point2DParameter<T>::Point2DParameter(const String & niceName, const String & description,
                                      T x, T y ,
                                      var minPoint,var maxPoint,
                                      bool enabled) :
MinMaxParameter( niceName, description,Array<var>{x,y},minPoint,maxPoint,enabled)
{
  if(!minimumValue.isUndefined() && ! minimumValue.isArray()){
    minimumValue = var::undefined();
    jassertfalse;
  }
  if(!maximumValue.isUndefined() && ! maximumValue.isArray()){
    maximumValue = var::undefined();
    jassertfalse;
  }
  hideInEditor = true;
  setPoint(x, y);
}

template<typename T>
void Point2DParameter<T>::setPoint(const Point<T> & _value)
{
  setPoint(_value.x, _value.y);
}

template<typename T>
void Point2DParameter<T>::setPoint(const T _x, const T _y)
{
  var d;
  d.append(_x);
  d.append(_y);
  setValue(d);
}

template<typename T>
void Point2DParameter<T>::setValueInternal(var & _value)
{

  if (!_value.isArray() || _value.size()!=2){
    jassertfalse;
    return;
  }
  if(!minimumValue.isUndefined()){
    _value.getArray()->set(0, jmax(static_cast<T>(minimumValue[0]),static_cast<T>(_value[0])));
    _value.getArray()->set(1, jmax(static_cast<T>(minimumValue[1]),static_cast<T>(_value[1])));
  }
  if(!maximumValue.isUndefined()){
    _value.getArray()->set(0, jmin(static_cast<T>(maximumValue[0]),static_cast<T>(_value[0])));
    _value.getArray()->set(1, jmin(static_cast<T>(maximumValue[1]),static_cast<T>(_value[1])));
  }
  Parameter::setValueInternal(_value);


}
template<typename T>
Point<T> Point2DParameter<T>::getPoint() {
  return Point<T>(getX(), getY());
}

template<typename T>
bool Point2DParameter<T>::checkValueIsTheSame(var newValue, var oldValue)
{
  if (!(newValue.isArray() && oldValue.isArray())) return false;
  return newValue[0] == oldValue[0] && newValue[1] == oldValue[1];
}

template class Point2DParameter<int>;
template class Point2DParameter<float>;
