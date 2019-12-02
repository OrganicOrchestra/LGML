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


const Identifier ControlAddressType::rootIdentifier(Controllable::toShortName("rootidentifier")); // need tobe

String ControlAddressType::toString()const {
    if(size()==0){return "/none";}
    String res;
    for(auto & i:*this){
        res+="/";
        res+=i.toString();
    }
    return res;
}

ControlAddressType ControlAddressType::fromString(const String & s){
    ControlAddressType res;
    StringArray sa;
    sa.addTokens(s, juce::StringRef ("/"), juce::StringRef ("\""));
    for(auto el:sa){
        if(el.isNotEmpty()){
            res.add(el);
        }
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
        while(insp!=maxParent && insp && insp->shortName!=rootIdentifier){
            res.add(insp->shortName);
            insp = insp->parentContainer;
        }
    }
    std::reverse(res.begin(), res.end());
#if JUCE_DEBUG
    res.cachedAddress=res.toString();
#endif
    return res;
}

ControlAddressType ControlAddressType::buildFromControllableContainer(const ControllableContainer * c,const ControllableContainer * maxParent){
    ControlAddressType res;

    const ControllableContainer * insp = c;
    while(insp!=maxParent&& insp && insp->shortName!=rootIdentifier){
        res.add(insp->shortName);
        insp = insp->parentContainer;
    }

    std::reverse(res.begin(), res.end());
#if JUCE_DEBUG
    res.cachedAddress=res.toString();
#endif
    return res;
}

ControllableContainer * ControlAddressType::resolveContainerFromContainer(const ControllableContainer *  c)const{

    if(size()==0){
        jassertfalse;
        return nullptr;
    }
    ControllableContainer * insp = c->getControllableContainerByShortName(getUnchecked(0));;
    int idx = 1;
    while(insp!=nullptr && idx<size()){
        insp = insp->getControllableContainerByShortName(getUnchecked(idx));
        idx++;
    }
    return insp;
}

Controllable * ControlAddressType::resolveControllableFromContainer(const ControllableContainer *  c)const{
    
    if(size()==0 || !c){
        jassertfalse;
        return nullptr;
    }
    const ControllableContainer  * insp = c;
    if(size()>1){
        auto parentAddress = *this;
        parentAddress.resize(size()-1);
        insp =parentAddress.resolveContainerFromContainer(c);
        if(!insp){
//#if JUCE_DEBUG
//            LOGW("can't resolve : " << parentAddress.toString() <<" in " << c->getControlAddress().toString() );
//#endif
            //            jassertfalse;
            return nullptr;
        }
    }

    return insp->getControllableByShortName(getUnchecked(size()-1));

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
#if JUCE_DEBUG
    res.cachedAddress=res.toString();
#endif
    return res;
}

StringArray ControlAddressType::toStringArray()const{
    StringArray sa;
    for(auto & s:*this){
        sa.add(s.toString());
    }
    return sa;
}


ControlAddressType ControlAddressType::getChild(const ShortNameType & c) const{
    ControlAddressType res;
    res = *this;
    res.add(c);
#if JUCE_DEBUG
    res.cachedAddress=res.toString();
#endif
    return res;
}

void ControlAddressType::add(const Identifier & i){
    Array<Identifier>::add(i);
#if JUCE_DEBUG
    cachedAddress=toString();
#endif
}
void ControlAddressType::set(const int i,const Identifier & idtf){
    Array<Identifier>::set(i,idtf);
#if JUCE_DEBUG
    cachedAddress=toString();
#endif
}
int ControlAddressType::size()const{
    return Array<Identifier>::size();
}

bool ControlAddressType::operator ==(const ControlAddressType & o) const{
    return getArray()==o.getArray();
}
bool ControlAddressType::operator !=(const ControlAddressType & o) const{
    return getArray()!=o.getArray();
}

const Array<Identifier> & ControlAddressType::getArray()const {
    return *(Array<Identifier>*)this;
}
////////////////////
//Controllable




ShortNameType Controllable::toShortName (const String& s){
    //        if (s.isEmpty()) return "";
    const String sn = s.retainCharacters("abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ0123456789_-.").toLowerCase();
    static ShortNameType emptyName("Empty");
    if(sn.isEmpty()) return emptyName;
    return ShortNameType(sn);
    //   #*,?[]{}/ based on OSC escaping
    // http://opensoundcontrol.org/spec-1_0
    // other for xml or generic escaping
    //        return ShortNameType(s.removeCharacters (" #*,?[]{}/:;%$<>()").toLowerCase());
}

Controllable::Controllable ( const String& _niceName, const String& _description, bool _enabled) :
description (_description),
parentContainer (nullptr),
isControllableExposed (true),
isHidenInEditor (false),
isSavable (true),
enabled (_enabled),
isUserDefined (false),
isSavableAsObject(false),
isPresettable(true)
{
    setEnabled (enabled);
    setNiceName (_niceName);
    
}


Controllable::~Controllable()
{
    Controllable::masterReference.clear();
    controllableListeners.call (&Controllable::Listener::controllableRemoved, this);

}


void Controllable::setNiceName (const String& _niceName)
{
    if(_niceName.isEmpty()){
        LOGE("no name given to controllable assigning to no name");
        jassertfalse;
        niceName = "no Name";
    }
    else if (niceName == _niceName){ return;}
    else{niceName = _niceName;}
    setAutoShortName();

}




void Controllable::setAutoShortName()
{
    shortName = toShortName (niceName);
    updateControlAddress(false);
    controllableListeners.call (&Listener::controllableNameChanged, this);
}


void Controllable::setEnabled (bool value, bool silentSet, bool force)
{
    if (!force && value == enabled) return;

    enabled = value;

    if (!silentSet) controllableListeners.call (&Listener::controllableStateChanged, this);
}


void Controllable::setParentContainer (ControllableContainer* container)
{
    parentContainer = container;
    updateControlAddress(true);
}


void Controllable::updateControlAddress(bool isParentResolved)
{
    if(isParentResolved && parentContainer){
        controlAddress=parentContainer->controlAddress.getChild(shortName);
#if JUCE_DEBUG
        //        jassert(Engine::getEngine() && ControllableContainer::globalRoot);
        if(Controllable::isChildOf(ControllableContainer::globalRoot)){
            auto tCs= ControlAddressType::buildFromControllable(this).toString();
            auto ccs = controlAddress.toString();
            jassert(tCs==ccs);
        }
#endif
    }
    else{
        controlAddress = ControlAddressType::buildFromControllable(this);
    }
    controllableListeners.call (&Listener::controllableControlAddressChanged, this);
}


ControlAddressType  Controllable::getControlAddressRelative (const ControllableContainer* relativeTo) const
{
    if(relativeTo!=nullptr){
        return ControlAddressType::buildFromControllable(this,relativeTo);
    }
    else{
#if JUCE_DEBUG
        jassert(controlAddress==ControlAddressType::buildFromControllable(this,relativeTo));
#endif
        return controlAddress;
    }

}

const ControlAddressType &  Controllable::getControlAddress () const
{

#if JUCE_DEBUG
    auto curAddr = ControlAddressType::buildFromControllable(this);
    if(controlAddress!=curAddr){
        auto curS = curAddr.toString();
        auto localS = controlAddress.toString();
        DBG(String("address mismatch ") + localS + " // " + curS );
        jassertfalse;
    }

#endif
    return controlAddress;


}

DynamicObject* Controllable::createDynamicObject()
{
    DynamicObject* dObject = new DynamicObject();
    JsHelpers::assignPtrToObject((Controllable*)this,dObject,false);
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


void Controllable::addControllableListener (Listener* newListener) { controllableListeners.add (newListener); }
void Controllable::removeControllableListener (Listener* listener) { controllableListeners.remove (listener); }
