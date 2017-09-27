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


#include "Parameter.h"
#include "../../Scripting/Js/JsHelpers.h"


const Identifier Parameter::valueIdentifier ("value");


Parameter::Parameter ( const String& niceName, const String& description, var initialValue, bool enabled) :
    Controllable ( niceName, description, enabled),
    isEditable (true),
    isPresettable (true),
    isOverriden (false),
    queuedNotifier (100),
    hasCommitedValue (false),
    isCommitableParameter (false),
    isSettingValue (false),
    isLocking (true),
    defaultValue (initialValue),
    value (initialValue),
    mappingDisabled (false)
{



}

void Parameter::resetValue (bool silentSet)
{
    isOverriden = false;
    setValue (defaultValue, silentSet, true);
}

void Parameter::setValue (var _value, bool silentSet, bool force)
{
    if (isCommitableParameter && !force)
    {
        commitValue (_value);
    }
    else
    {
        tryToSetValue (_value, silentSet, force);
    }

}

bool Parameter::waitOrDeffer (const var& _value, bool silentSet, bool force )
{
    if (!force && isSettingValue)
    {
        if (isLocking)
        {
            int overflow = 1000000;
            auto startWait = Time::currentTimeMillis();

            while (isSettingValue && overflow > 0)
            {
                //        Thread::sleep(1);
                Thread::yield();
                overflow--;
            }

            if (isSettingValue && overflow <= 0)
            {
                DBG ("locked for : " << Time::currentTimeMillis() - startWait);
            }
        }

        // force defering if locking too long or not locking
        if (isSettingValue)
        {
            if (auto* mm = MessageManager::getInstanceWithoutCreating())
            {
                mm->callAsync ([this, _value, silentSet, force]() {tryToSetValue (_value, silentSet, force);});
                return true;
            }
        }

        //    jassertfalse;
    }

    return false;
}
void Parameter::tryToSetValue (var _value, bool silentSet, bool force )
{

    if (!force && checkValueIsTheSame (_value, value)) return;

    if (!waitOrDeffer (_value, silentSet, force))
    {
        isSettingValue = true;

        if (value.getDynamicObject())
        {
            lastValue = value.clone();
        }
        else
        {
            lastValue = var (value);
        }

        setValueInternal (_value);

        if (_value != defaultValue) isOverriden = true;

        if (!silentSet && !checkValueIsTheSame (lastValue, value)) notifyValueChanged (false);

        isSettingValue = false;
    }

}


void Parameter::commitValue (var _value)
{
    hasCommitedValue = value != _value;
    commitedValue  = _value;

}

void Parameter::pushValue (bool force)
{
    if (!hasCommitedValue && !force)return;

    tryToSetValue (commitedValue, false, true);
    hasCommitedValue = false;
}


void Parameter::setValueInternal (var& _value) //to override by child classes
{

    value = _value;
#ifdef JUCE_DEBUG
    checkVarIsConsistentWithType();
#endif
}

bool Parameter::checkValueIsTheSame (const var& v1, const var& v2)
{
    return v1.hasSameTypeAs (v2) && (v1 == v2);
}

void Parameter::checkVarIsConsistentWithType()
{
    //  if      (type ==  && !value.isString()) { value = value.toString();}
    //  else if (type == Type::INT && !value.isInt())       { value = int(value);}
    //  else if (type == Type::BOOL && !value.isBool())     { value = bool(value);}
    //  else if (type == Type::FLOAT && !value.isDouble())  { value = double(value);}
    //  else if (type == Type::POINT2D && !value.isArray()) { value = Array<var>{0,0};}
    //  else if (type == Type::POINT3D && !value.isArray()) { value = Array<var>{0,0,0};}
}



void Parameter::notifyValueChanged (bool defferIt)
{
    if (defferIt)
        triggerAsyncUpdate();
    else
        listeners.call (&Listener::parameterValueChanged, this);

    queuedNotifier.addMessage (new ParamWithValue (this, value, false));
}


//JS Helper


DynamicObject* Parameter::createDynamicObject()
{
    auto dObject = Controllable::createDynamicObject();
    static const Identifier _jsSetIdentifier ("set");
    dObject->setMethod (_jsSetIdentifier, setControllableValueFromJS);
    return dObject;
}

void Parameter::configureFromObject (DynamicObject* ob)
{
    if (ob)
    {
        if (ob->hasProperty ("initialValue")) {defaultValue = ob->getProperty ("initialValue");}

        if (ob->hasProperty ("value")) {setValue (ob->getProperty ("value"));}
    }
    else
    {
        jassertfalse;
    }
}

DynamicObject* Parameter::getObject()
{
    DynamicObject* res = new DynamicObject();
    res->setProperty (valueIdentifier, value);
    return res;
}

var Parameter::getVarState()
{
    return value;
}



void Parameter::handleAsyncUpdate()
{
    listeners.call (&Listener::parameterValueChanged, this);
};

bool Parameter::isMappable()
{
    return isEditable && !mappingDisabled ;
}

void Parameter::setStateFromVar (const var& v)
{
    setValue (v);
}
