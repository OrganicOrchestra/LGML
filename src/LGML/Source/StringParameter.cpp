/*
 ==============================================================================

 StringParameter.cpp
 Created: 9 Mar 2016 12:29:30am
 Author:  bkupe

 ==============================================================================
 */

#include "StringParameter.h"



StringParameter::StringParameter(const String & niceName, const String &description, const String & initialValue, bool enabled) :
Parameter(Type::STRING, niceName, description, initialValue, var(), var(), enabled)
{

}


void StringParameter::tryToSetValue(var _value,bool silentSet,bool force,bool defferIt ){


  if (!force && value.toString() == _value.toString()) return;
  jassert(isSettingValue==false);
  isSettingValue = true;
  setValueInternal(_value);


  if(_value != defaultValue) isOverriden = true;

  if (!silentSet) notifyValueChanged(defferIt);
  isSettingValue = false;
};

void  StringParameter::setValueInternal(var & newVal){
  value = newVal.toString();
};

