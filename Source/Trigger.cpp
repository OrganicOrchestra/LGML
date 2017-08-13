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


#include "Trigger.h"



#include "JsHelpers.h"


const var Trigger::triggerVar;

Trigger::Trigger(const String & niceName, const String &description, bool enabled) :
Parameter(TRIGGER, niceName, description, enabled,false,false,true)
{
  Controllable::isSavable = false;
}






DynamicObject * Trigger::createDynamicObject()
{
	DynamicObject* dObject = Controllable::createDynamicObject();
	dObject->setMethod(jsTriggerIdentifier, setControllableValueFromJS);
	dObject->setProperty(jsPtrIdentifier, (int64)this);

	return dObject;
}
void Trigger::setFromVarObject(DynamicObject & ){
  jassertfalse;
}

var Trigger::getVarObject(){
  var res = new DynamicObject();
  return res;
}
var Trigger::getVarState(){
  // isSavable should be false
  jassertfalse;
  return var::undefined();
}
