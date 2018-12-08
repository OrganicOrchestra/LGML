/*
 ==============================================================================

 Copyright © Organic Orchestra, 2017

 This file is part of LGML. LGML is a software to manipulate sound in real-time

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

String ControlAddressType::toString()const {
    if(size()==0){return "/none";}
    String res;
    for(auto & i:*this){
        res+="/";
        res+=i.toString();
    }
    return res;
}

ControlAddressType ControlAddressType::buildFromControllable(const Controllable * c,const ControllableContainer * maxParent){
    ControlAddressType res;
    static Identifier noParentId("noParent");
    res.add(c->shortName);
    ControllableContainer * insp = c->parentContainer;
    if(!insp){
        res.add(noParentId);
    }
    else{
        while(insp!=maxParent){
            res.add(insp->shortName);
            insp = insp->parentContainer;
        }
    }
    std::reverse(res.begin(), res.end());
    return res;
}

ControlAddressType ControlAddressType::buildFromControllableContainer(const ControllableContainer * c,const ControllableContainer * maxParent){
    ControlAddressType res;
    ControllableContainer * insp = c->parentContainer;
    if(!insp){
        return res;
    }
    else{
        res.add(c->shortName);
        while(insp!=maxParent){
            res.add(insp->shortName);
            insp = insp->parentContainer;
        }
    }
    std::reverse(res.begin(), res.end());
    return res;
}

ControllableContainer * ControlAddressType::resolveContainerFromContainer(const ControllableContainer *  c)const{

    if(size()==0){
        jassertfalse;
        return nullptr;
    }
    ControllableContainer * insp = c->getControllableContainerByShortName(this->getUnchecked(0));;
    int idx = 1;
    while(insp!=nullptr && idx<size()){
        insp = insp->getControllableContainerByShortName(this->getUnchecked(idx));
        idx++;
    }
    return insp;
}

Controllable * ControlAddressType::resolveControllableFromContainer(const ControllableContainer *  c)const{
    
    if(size()==0){
        jassertfalse;
        return nullptr;
    }
    auto parentAddress = *this;
    parentAddress.resize(size()-1);
    ControllableContainer  * insp =parentAddress.resolveContainerFromContainer(c);
    return insp->getControllableByShortName(this->getUnchecked(size()-1));

}

ControlAddressType ControlAddressType::getRelativeTo(ControlAddressType & other)const{
    int comonIdx = 0;
    while(comonIdx<size() && comonIdx<other.size() && getUnchecked(comonIdx)==other.getUnchecked(comonIdx)){
        comonIdx++;
    }
    return subAddr(comonIdx);
}

ControlAddressType ControlAddressType::subAddr(int start, int end)const{
    if(end == -1) end=size();
    jassert(start<size());
    ControlAddressType res;
    for(int i = start ; i < end ; i++){
        res.add(getUnchecked(i));
    }
    return res;
}

StringArray ControlAddressType::toStringArray()const{
    StringArray sa;
    for(auto & s:*this){
        sa.add(s.toString());
    }
    return sa;
}

////////////////////
//

Controllable::Controllable ( const String& niceName, const String& description, bool enabled) :
    description (description),
    parentContainer (nullptr),
    isControllableExposed (true),
    isHidenInEditor (false),
    shouldSaveObject (false),
    isSavable (true),
    enabled (enabled),
    isUserDefined (false),
    isSavableAsObject(false)
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
    controlAddress = ControlAddressType::buildFromControllable(this);
    listeners.call (&Listener::controllableControlAddressChanged, this);
}


const ControlAddressType & Controllable::getControlAddress (const ControllableContainer* relativeTo) const
{
#if JUCE_DEBUG
    jassert(controlAddress==ControlAddressType::buildFromControllable(this,relativeTo));
#endif
    return controlAddress;
}



DynamicObject* Controllable::createDynamicObject()
{
    DynamicObject* dObject = new DynamicObject();
    JsHelpers::assignPtrToObject((Controllable*)this,dObject,true);
    dObject->setMethod (JsHelpers::jsGetIdentifier, Controllable::getVarStateFromScript);
    return dObject;
}



var Controllable::getVarStateFromScript (const juce::var::NativeFunctionArgs& a)
{

    Controllable* c = JsHelpers::castPtrFromJS<Controllable> (a);

    if (c == nullptr  ) return var();

    //  WeakReference<ParameterBase> wc = c;
    //  if(!wc.get()) return var();
    return c->getVarState();

}
//STATIC


var Controllable::setControllableValueFromJS (const juce::var::NativeFunctionArgs& a)
{

    Controllable* c = JsHelpers::castPtrFromJS<Controllable> (a);
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
        LOGE (juce::translate("unknown controllable set from js"));
        jassertfalse;
    }

    return var();
}






bool Controllable::isMappable()
{
    return false;
}


bool Controllable::isChildOf (const ControllableContainer* p) const
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
