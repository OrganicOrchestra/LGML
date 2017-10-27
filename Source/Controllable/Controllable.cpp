/*
 ==============================================================================

 Copyright © Organic Orchestra, 2017

 This file is part of LGML. LGML is a software to manipulate sound in realtime

 This program is free software; you can redistribute it and/or modify
 it under the terms of the GNU General Public License as published by
 the Free Software Foundation (version 3 of the License).

 This program is distributed in the hope that it will be useful,
 but WITHOUT ANY WARRANTY; without even the implied warranty of
 MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.

 ==============================================================================
 */

#include "Controllable.h"
#include "ControllableContainer.h"
#include "../Scripting/Js/JsHelpers.h"
#include "../Utils/StringUtil.hpp"



Controllable::Controllable ( const String& niceName, const String& description, bool enabled) :
    description (description),
    parentContainer (nullptr),
    isControllableExposed (true),
    isHidenInEditor (false),
    shouldSaveObject (false),
    isSavable (true),
    enabled (enabled),
    isUserDefined (false)
{
    setEnabled (enabled);
    setNiceName (niceName);
}


Controllable::~Controllable()
{
    Controllable::masterReference.clear();
    listeners.call (&Controllable::Listener::controllableRemoved, this);

}


void Controllable::setNiceName (const String& _niceName)
{
    if (niceName == _niceName) return;

    niceName = _niceName;

    setAutoShortName();

}




void Controllable::setAutoShortName()
{
    shortName = toShortName (niceName);
    updateControlAddress();
    listeners.call (&Listener::controllableNameChanged, this);
}


void Controllable::setEnabled (bool value, bool silentSet, bool force)
{
    if (!force && value == enabled) return;

    enabled = value;

    if (!silentSet) listeners.call (&Listener::controllableStateChanged, this);
}


void Controllable::setParentContainer (ControllableContainer* container)
{
    this->parentContainer = container;
    updateControlAddress();
}


void Controllable::updateControlAddress()
{
    this->controlAddress = getControlAddress();
    listeners.call (&Listener::controllableControlAddressChanged, this);
}


String Controllable::getControlAddress (ControllableContainer* relativeTo)
{
    // we may need empty parentContainer in unit tests
#if LGML_UNIT_TESTS
    return (parentContainer ? parentContainer->getControlAddress (relativeTo) : "") + "/" + shortName;
#else
    return parentContainer->getControlAddress (relativeTo) + "/" + shortName;
#endif
}



DynamicObject* Controllable::createDynamicObject()
{
    DynamicObject* dObject = new DynamicObject();
    dObject->setProperty (jsPtrIdentifier, (int64)this);
    //  dObject->setProperty(jsVarObjectIdentifier, getVarObject());
    dObject->setMethod (jsGetIdentifier, Controllable::getVarStateFromScript);
    return dObject;
}



var Controllable::getVarStateFromScript (const juce::var::NativeFunctionArgs& a)
{
    // TODO handle with weak references
    Controllable* c = getObjectPtrFromJS<Controllable> (a);

    if (c == nullptr  ) return var();

    //  WeakReference<Parameter> wc = c;
    //  if(!wc.get()) return var();
    return c->getVarState();

}
//STATIC


var Controllable::setControllableValueFromJS (const juce::var::NativeFunctionArgs& a)
{

    Controllable* c = getObjectPtrFromJS<Controllable> (a);
    //    bool success = false;

    if (c != nullptr)
    {
        //      success = true;
        var value = a.numArguments == 0 ? var::undefined() : a.arguments[0];
        c->setStateFromVar (value);
        return c->getVarState();
    }
    else
    {
        LOG ("!!!unknown controllable set from js");
        jassertfalse;
    }

    return var();
}






bool Controllable::isMappable()
{
    return false;
}


bool Controllable::isChildOf (ControllableContainer* p)
{
    auto i = parentContainer;

    while (i)
    {
        if (i == p)
        {
            return true;
        }

        i = i->parentContainer;
    }

    return false;

}
