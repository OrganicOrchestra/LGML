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


#include "Point3DParameter.h"

Point3DParameter::Point3DParameter(const String & niceName, const String & description, bool enabled) :
	MinMaxParameter( niceName, description, 0, 0, 1, enabled)
{
}

void Point3DParameter::setPoint(Vector3D<float> _value)
{
	var d;
	d.append(_value.x);
	d.append(_value.y);
	d.append(_value.z);
	setValue(d);
}

void Point3DParameter::setValueInternal(var & _value)
{
	if (!_value.isArray()) return;
	x = _value[0];
	y = _value[1];
}

Vector3D<float> * Point3DParameter::getPoint() {
	return new Vector3D<float>(x, y,z);
}

bool Point3DParameter::checkValueIsTheSame(const var & v1, const var& v2)
{
	if (!(v1.isArray() && v2.isArray())) return false;
	return v1[0] == v2[0] && v1[1] == v2[1] && v1[2] == v2[2];
}
