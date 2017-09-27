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


#ifndef POINT3DPARAMETER_H_INCLUDED
#define POINT3DPARAMETER_H_INCLUDED



#include "MinMaxParameter.h"
//#include <juce_opengl/juce_opengl.h>

class Point3DParameter : public MinMaxParameter
{
public:
    Point3DParameter (const String& niceName, const String& description, bool enabled = true);
    ~Point3DParameter() {}

    float x, y, z;
    //  void setPoint(Vector3D<float> value);
    void setValueInternal (var& _value) override;

    //  Vector3D<float> * getPoint();

    bool checkValueIsTheSame (const var& v1, const var& v2) override;


    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (Point3DParameter)
};


#endif  // POINT3DPARAMETER_H_INCLUDED
