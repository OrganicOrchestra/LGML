/*
  ==============================================================================

    Point2DParameter.cpp
    Created: 30 Sep 2016 9:37:36am
    Author:  bkupe

  ==============================================================================
*/

#include "Point2DParameter.h"

Point2DParameter::Point2DParameter(const String & niceName, const String & description, bool enabled) :
	Parameter(POINT2D, niceName, description,0,0,1,enabled)
{
	hideInEditor = true;
	setPoint(0, 0);
}

void Point2DParameter::setPoint(Point<float> _value)
{
	setPoint(_value.x, _value.y);
}

void Point2DParameter::setPoint(float _x, float _y)
{
	var d;
	d.append(_x);
	d.append(_y);
	setValue(d);
}

void Point2DParameter::setValueInternal(var & _value)
{
	if (!_value.isArray()) return;
	x = _value[0];
	y = _value[1];
}

Point<float> Point2DParameter::getPoint() {
	return Point<float>(x, y); 
}

bool Point2DParameter::checkValueIsTheSame(var newValue, var oldValue)
{
	if (!(newValue.isArray() && oldValue.isArray())) return false;

	return newValue[0] == oldValue[0] && newValue[1] == oldValue[1];
}
