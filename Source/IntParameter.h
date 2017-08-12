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


#ifndef INTPARAMETER_H_INCLUDED
#define INTPARAMETER_H_INCLUDED

#include "Parameter.h"


class IntParameter : public Parameter
{
public:
    IntParameter(const String &niceName, const String &description, const int &initialValue=0, const int &minimumValue = 0, const int &maximumValue = 1, bool enabled = true);
    ~IntParameter() {}
  

    void setValueInternal(var & _value) override;



    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(IntParameter)
};


#endif  // INTPARAMETER_H_INCLUDED
