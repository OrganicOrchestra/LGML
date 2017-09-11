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


#include "StringParameter.h"

#include "ParameterFactory.h"
REGISTER_PARAM_TYPE(StringParameter)


StringParameter::StringParameter(const String & niceName, const String &description, const String & initialValue, bool enabled) :
Parameter(Type::STRING, niceName, description, initialValue, enabled)
{

}


void StringParameter::tryToSetValue(var _value,bool silentSet,bool force ){


  if (!force && value.toString() == _value.toString()) return;
  if(!waitOrDeffer(_value, silentSet, force)){
    isSettingValue = true;
    setValueInternal(_value);
    if(_value != defaultValue) isOverriden = true;
    if (!silentSet) notifyValueChanged();
    isSettingValue = false;
  }
};

void  StringParameter::setValueInternal(var & newVal){
  value = newVal.toString();
};

