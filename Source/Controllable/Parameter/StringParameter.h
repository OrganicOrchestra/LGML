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


#ifndef STRINGPARAMETER_H_INCLUDED
#define STRINGPARAMETER_H_INCLUDED


#include "Parameter.h"


class StringParameter : public Parameter
{
public:
  StringParameter(const String &niceName, const String &description=String::empty, const String &initialValue=String::empty, bool enabled=true);



  // need to override this function because var Strings comparison  is based on pointer (we need full string comp)
  bool checkValueIsTheSame(const var & v1,const var & v2)override;
  void setValueInternal(var&)override;

  bool isMappable() override;
  DECLARE_OBJ_TYPE(StringParameter)
  JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(StringParameter)
};



#endif  // STRINGPARAMETER_H_INCLUDED
