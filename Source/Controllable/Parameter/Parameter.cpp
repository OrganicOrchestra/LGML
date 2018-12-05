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
#include "../../Utils/DebugHelpers.h"


const Identifier ParameterBase::valueIdentifier ("value");


ParameterBase::ParameterBase ( const String& niceName, const String& description, var initialValue, bool enabled) :
    Controllable ( niceName, description, enabled),
    isEditable (true),
    isPresettable (true),
    isOverriden (false),
    queuedNotifier (100),
    hasCommitedValue (false),
    isCommitableParameter (false),
    _isSettingValue (false),
    isLocking (true),
    defaultValue (initialValue),
    value (initialValue),
    mappingDisabled (false),
    alwaysNotify(false)
{



}

void ParameterBase::resetValue (bool silentSet,bool force)
{
    isOverriden = false;
    setValue (defaultValue, silentSet, force);
}

void ParameterBase::setNewDefault(const var & v,bool notify){
    defaultValue=v;
    if(!isOverriden){
        resetValue(!notify,false);
    }

}

void ParameterBase::setValue (const var & _value, bool silentSet, bool force)
{
    if (isCommitableParameter && !force)
    {
        commitValue (_value);
    }
    else
    {
        tryToSetValue (_value, silentSet, alwaysNotify || force);
    }

}
void ParameterBase::setValueFrom(Listener * notifier,const var & _value, bool silentSet , bool force ){
    jassert(!isCommitableParameter);
    // reentrancy check
    if(notifier !=nullptr && (_valueSetter.get()==notifier) ) force=true;
    _valueSetter = notifier;
     tryToSetValue (_value, silentSet, alwaysNotify || force,notifier);
    _valueSetter = nullptr;

}

bool ParameterBase::waitOrDeffer (const var& _value, bool silentSet, bool force )
{
    if (!force && _isSettingValue.get())
    {
        if (isLocking)
        {
            int overflow = 1000000;
            auto startWait = Time::currentTimeMillis();

            while (_isSettingValue.get() && overflow > 0)
            {
                //        Thread::sleep(1);
                Thread::yield();
                overflow--;
            }

            if (_isSettingValue.get() && overflow <= 0)
            {
                LOGE("param " << controlAddress << " locked for : " << Time::currentTimeMillis() - startWait);
            }
        }

        // force defering if locking too long or not locking
        if (_isSettingValue.get())
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
void ParameterBase::tryToSetValue (const var & _value, bool silentSet, bool force,Listener * notifier )
{

    if (!force && checkValueIsTheSame (_value, value)) return;

    if (!waitOrDeffer (_value, silentSet, force))
    {
        _isSettingValue = true;
        lastValue = value.clone();
        setValueInternal (_value);

        if (!isOverriden && !checkValueIsTheSame (defaultValue, value)) isOverriden = true;

        if (!silentSet && (force || !checkValueIsTheSame (lastValue, value)))
            notifyValueChanged (false,notifier);

        _isSettingValue = false;
    }

}


void ParameterBase::commitValue (var _value)
{
    hasCommitedValue = value != _value;
    commitedValue  = _value.clone();

}

void ParameterBase::pushValue (bool force)
{
    if (!hasCommitedValue && !force)return;

    tryToSetValue (commitedValue, false, true);
    hasCommitedValue = false;
}


void ParameterBase::setValueInternal (const var& _value) //to override by child classes
{

    value = _value;
#ifdef JUCE_DEBUG
    checkVarIsConsistentWithType();
#endif
}

bool ParameterBase::checkValueIsTheSame (const var& v1, const var& v2)
{
    return v1.hasSameTypeAs (v2) && (v1 == v2);
}

void ParameterBase::checkVarIsConsistentWithType()
{
    //  if      (type ==  && !value.isString()) { value = value.toString();}
    //  else if (type == Type::INT && !value.isInt())       { value = int(value);}
    //  else if (type == Type::BOOL && !value.isBool())     { value = bool(value);}
    //  else if (type == Type::FLOAT && !value.isDouble())  { value = double(value);}
    //  else if (type == Type::POINT2D && !value.isArray()) { value = Array<var>{0,0};}
    //  else if (type == Type::POINT3D && !value.isArray()) { value = Array<var>{0,0,0};}
}



void ParameterBase::notifyValueChanged (bool defferIt,Listener * notifier)
{
    if (defferIt)
        triggerAsyncUpdate();
    else{
        // call all listeners as they still need to dispatch feedback
        listeners.call (&Listener::parameterValueChanged, this,notifier);
    }

    queuedNotifier.addMessage (new ParamWithValue (this, value, false,notifier));
}


//JS Helper


DynamicObject* ParameterBase::createDynamicObject()
{
    auto dObject = Controllable::createDynamicObject();
    static const Identifier _jsSetIdentifier ("set");
    dObject->setMethod (_jsSetIdentifier,
                        setControllableValueFromJS
                        );
    return dObject;
}

void ParameterBase::configureFromObject (DynamicObject* ob)
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

DynamicObject* ParameterBase::createObject()
{
    DynamicObject* res = new DynamicObject();
    res->setProperty (valueIdentifier, value);
    return res;
}

var ParameterBase::getVarState()
{
    return value;
}



void ParameterBase::handleAsyncUpdate()
{
    listeners.call (&Listener::parameterValueChanged, this,nullptr);
};

bool ParameterBase::isMappable()
{
    return isEditable && !mappingDisabled ;
}

void ParameterBase::setStateFromVar (const var& v)
{
    setValue (v);
}

bool ParameterBase::isSettingValue(){
    return _isSettingValue.get();
}
