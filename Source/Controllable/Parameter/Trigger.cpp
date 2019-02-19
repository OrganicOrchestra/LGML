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



#include "../../Scripting/Js/JsHelpers.h"
#include "ParameterFactory.h"

const var Trigger::triggerVar (true);


REGISTER_PARAM_TYPE (Trigger);

Trigger::Trigger (const String& niceName, const String& description = "", bool enabled) :
   ParameterBase ( niceName, description, triggerVar, enabled)
{
    setSavable(false);
    lastTime = 0;
}


void Trigger::tryToSetValue (const var & _value, bool silentSet, bool force,Listener * notifier){

    if (!shouldBeDeffered (_value, silentSet, force))
    {
        if ( (bool) _value)
        {

            _isSettingValue = true;

            if (!silentSet) notifyValueChanged(false,notifier);

            _isSettingValue = false;
        }
    }

}



DynamicObject* Trigger::createDynamicObject()
{
    auto dObject = Controllable::createDynamicObject();
    dObject->setMethod (JsHelpers::jsTriggerIdentifier, setControllableValueFromJS);

    return dObject;
}

DynamicObject* Trigger::createObject()
{
    return new DynamicObject();
}
var Trigger::getVarState()
{
    // isSavable should be false
//    jassertfalse;
    return var::undefined();
}
