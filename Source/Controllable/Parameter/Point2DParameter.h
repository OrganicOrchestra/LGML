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


#ifndef POINT2DPARAMETER_H_INCLUDED
#define POINT2DPARAMETER_H_INCLUDED

#include "MinMaxParameter.h"

template<typename T>
class Point2DParameter : public MinMaxParameter
{
public:
  Point2DParameter(const String &niceName, const String &description=String::empty, T x= T(0), T y = T(0),var minPoint=var::undefined(),var maxPoint=var::undefined(), bool enabled = true);
	~Point2DParameter() {}

  


	void setPoint(const Point<T> & value);
	void setPoint(const T x, const T y);
	void setValueInternal(var & _value) override;
	
	Point<T> getPoint();
	
	bool checkValueIsTheSame(var newValue, var oldValue) override;
  T getX(){return (T)value[0];}
  T getY(){return (T)value[1];}
  DECLARE_OBJ_TYPE(Point2DParameter<T>);
private:

	
	JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(Point2DParameter)
};





#endif  // POINT2DPARAMETER_H_INCLUDED
