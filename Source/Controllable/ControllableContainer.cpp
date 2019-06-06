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

#include "ControllableContainer.h"


#include "../Utils/DebugHelpers.h"
#include "../Scripting/Js/JsHelpers.h"
#include "Parameter/ParameterFactory.h"

#include <juce_osc/juce_osc.h>


ControllableContainer * ControllableContainer::globalRoot(nullptr);
const Identifier ControllableContainer::controlAddressIdentifier ("controlAddress");

const Identifier ControllableContainer::childContainerId ("/");

const Identifier ControllableContainer::controllablesId ("parameters");



ControllableContainer::ControllableContainer (StringRef niceName) :
parentContainer (nullptr),
numContainerIndexed (0),
localIndexedPosition (-1),
isUserDefined (false),
canHaveUserDefinedContainers(false)
{

    shortName = Controllable::toShortName (niceName);
    controlAddress = ControlAddressType::buildFromControllableContainer(this);

}


ControllableContainer::~ControllableContainer()
{
    clearContainer(true);
    ControllableContainer::masterReference.clear();
}
ControllableContainer * ControllableContainer::getRoot(bool global){
    if(global){
        jassert(globalRoot);
        return globalRoot;
    }
    else{
        ControllableContainer * pc = this;
        while(pc->parentContainer){
            pc = pc->parentContainer;
        }
        return pc;
    }
}

void ControllableContainer::clearContainer(bool doDeleteChilds)
{
    controllableContainerListeners.call (&Listener::containerWillClear, this);

    while (controllables.size())
    {
        removeControllable (controllables[0]);
    }
    if(doDeleteChilds){
        for(auto c : controllableContainers){
            if(c.get()){
                c->clearContainer(true);
                delete c;}
        }
    }
    controllableContainers.clear();

}


void ControllableContainer::removeFromParent()
{
    jassert (parentContainer);

    if (parentContainer)
    {
        parentContainer->removeChildControllableContainer (this);
    }
}



void ControllableContainer::removeControllable (Controllable* c)
{
    controllableContainerListeners.call (&Listener::childControllableRemoved, this, c);


    controllables.removeObject (c);
    notifyStructureChanged (this,false,true,false);
}

void ControllableContainer::addControllable(Controllable *c){
    controllables.add(c);
    notifyStructureChanged (this,true,true,false);
}

void ControllableContainer::notifyStructureChanged (ControllableContainer* origin,bool isAdded,bool controllableUpdated, bool containerUpdated)
{

    controllableContainerListeners.call (&Listener::childStructureChanged, this, origin,isAdded);

    if (parentContainer)
    {
        parentContainer->notifyStructureChanged (origin,isAdded,controllableUpdated,containerUpdated);
    }
}

void ControllableContainer::notifyChildAddressChanged (ControllableContainer* origin)
{

    controllableContainerListeners.call (&Listener::childAddressChanged, this, origin);

    if (parentContainer)
    {
        parentContainer->notifyChildAddressChanged (origin);
    }
}


String ControllableContainer::setNiceName (const String& _niceName)
{
    String targetName (_niceName);

    if (parentContainer)
    {
        targetName = parentContainer->getUniqueNameInContainer (_niceName, 0, this);
    }

    return targetName;

}




void ControllableContainer::setAutoShortName()
{
    shortName = Controllable::toShortName (getNiceName());
    updateControlAddress(true);
    notifyChildAddressChanged(this);
}



Controllable* ControllableContainer::getControllableByName (const String& _name) const
{
    ScopedLock lk (controllables.getLock());

    for (auto * c : controllables)
    {
        if(c->niceName == _name) {return c;}
    }
    

    return nullptr;
}

Controllable* ControllableContainer::getControllableByShortName(const ShortNameType & n) const{
    ScopedLock lk (controllables.getLock());
    for (auto * c : controllables)
    {
        if (c->shortName==n) return c;
    }
    return nullptr;
}

Controllable* ControllableContainer::getControllableByShortName(const String & _name) const{
    const ShortNameType sname = Controllable::toShortName(_name);
    return getControllableByShortName(sname);
}

ControllableContainer* ControllableContainer::addChildControllableContainer (ControllableContainer* container, bool notify)
{
    String oriName = container->getNiceName();
    String targetName = getUniqueNameInContainer (oriName);

    if (targetName != oriName)
    {
        container->setNiceName (targetName);
    }

    controllableContainers.add (container);
    //  container->addControllableContainerListener(this);
    jassert(container->parentContainer==nullptr || container->parentContainer==this);
    container->setParentContainer (this);

    if (notify)
    {
        controllableContainerListeners.call (&Listener::controllableContainerAdded, this, container);
        notifyStructureChanged (this,true,false,true);
    }

    return container;
}


void ControllableContainer::removeChildControllableContainer (ControllableContainer* container)
{
    if (numContainerIndexed > 0 &&
        container->localIndexedPosition >= 0 &&
        controllableContainers.getUnchecked (container->localIndexedPosition) == container)
    {
        numContainerIndexed--;
    }



    controllableContainerListeners.call (&Listener::controllableContainerRemoved, this, container);
    controllableContainers.removeAllInstancesOf (container);


    notifyStructureChanged (this,false,false,true);
    container->setParentContainer (nullptr);
}


void ControllableContainer::addChildIndexedControllableContainer (ControllableContainer* container, int idx)
{
    if (idx == -1 )idx = numContainerIndexed;

    jassert (idx <= numContainerIndexed);

    controllableContainers.insert (idx, container);
    container->localIndexedPosition = idx;
    numContainerIndexed++;

    //  container->addControllableContainerListener(this);
    container->setParentContainer (this);
    controllableContainerListeners.call (&Listener::controllableContainerAdded, this, container);
    notifyStructureChanged (this,true,false,true);
}


void ControllableContainer::removeChildIndexedControllableContainer (int idx)
{
    if (idx == -1 )idx = numContainerIndexed - 1;

    jassert (idx < numContainerIndexed);


    removeChildControllableContainer (controllableContainers.getUnchecked (idx));
    numContainerIndexed--;

    for (int i = idx ; i < numContainerIndexed ; i ++)
    {
        controllableContainers.getUnchecked (i)->localIndexedPosition = i;
        controllableContainers.getUnchecked (i)->localIndexChanged();
    }

}


int ControllableContainer::getNumberOfIndexedContainer() {return numContainerIndexed;}

int ControllableContainer::getIndexedPosition() {return localIndexedPosition;}

bool ControllableContainer::hasIndexedContainers() {return numContainerIndexed > 0;}

bool ControllableContainer::isIndexedContainer() {return localIndexedPosition >= 0;}

void ControllableContainer::localIndexChanged() {};

ControllableContainer* ControllableContainer::getControllableContainerByName (const String& _name) const
{

    ScopedLock lk (controllableContainers.getLock());

    for (auto & c : controllableContainers)
    {
        if(c && c->getNiceName() == _name) {return c;}
    }
    return nullptr;

}


ControllableContainer* ControllableContainer::getControllableContainerByShortName (const String & _name) const{
const auto name = Controllable::toShortName(_name);
    return getControllableContainerByShortName(name);
}
ControllableContainer* ControllableContainer::getControllableContainerByShortName (const ShortNameType & name) const{
    ScopedLock lk (controllableContainers.getLock());
    for (auto& cc : controllableContainers)
    {
        if (cc.get() && (cc->shortName==name )) return cc;
    }
    return nullptr;
}

ControllableContainer * ControllableContainer::getMirroredContainer(ControllableContainer * other,ControllableContainer * root ){
    if(other==root){return this;}
    ControlAddressType relAddr = other->controlAddress.getRelativeTo(root->controlAddress);
    if(relAddr.size()==0){
        jassertfalse;
        return nullptr;
    }
    return relAddr.resolveContainerFromContainer(this);

}

ControllableContainer* ControllableContainer::findFirstControllableContainer (const std::function<bool(ControllableContainer*)> fun) const{

    for(auto & c : controllableContainers){
        if(c.get()){
        if(fun(c))return c;
        if(auto ch = c->findFirstControllableContainer(fun)){
            return ch;
        }
        }
    }


    return nullptr;

}


ControlAddressType  ControllableContainer::getControlAddressRelative (const ControllableContainer* relativeTo) const
{
    return ControlAddressType::buildFromControllableContainer(this,relativeTo);
}

const ControlAddressType & ControllableContainer::getControlAddress () const
{
#if JUCE_DEBUG


    auto curAddr = ControlAddressType::buildFromControllableContainer(this);
    if(controlAddress!=curAddr){
        auto curS = curAddr.toString();
        auto localS = controlAddress.toString();
        DBG(String("address mismatch ") + localS + " // " + curS );
        jassertfalse;
    }

#endif
    return controlAddress;
}



void ControllableContainer::setParentContainer (ControllableContainer* container)
{
    parentContainer = container;
    if(container)updateControlAddress(true);

}
void ControllableContainer::updateControlAddress(bool isParentResolved){
    if(isParentResolved && parentContainer){
        controlAddress = parentContainer->controlAddress.getChild(shortName);
    }
    else{
        controlAddress = ControlAddressType::buildFromControllableContainer(this);
    }
    if(parentContainer!=nullptr)updateChildrenControlAddress();
}

void ControllableContainer::updateChildrenControlAddress()
{
    {
        ScopedLock lk (controllables.getLock());

        for (auto& c : controllables) c->updateControlAddress(true);
    }
    {
        ScopedLock lk (controllableContainers.getLock());

        for (auto& cc : controllableContainers) if (cc.get())cc->updateControlAddress(true);
    }


}


Array<WeakReference<Controllable> > ControllableContainer::getAllControllables (bool recursive, bool getNotExposed)
{

    Array<WeakReference<Controllable>> result;
    {
        ScopedLock lk (controllables.getLock());

        for (const auto& c : controllables)
        {
            if (getNotExposed || c->isControllableExposed) result.add (c);
        }
    }

    if (recursive)
    {
        ScopedLock lk (controllableContainers.getLock());

        for (auto& cc : controllableContainers) if (cc.get())result.addArray (cc->getAllControllables (true, getNotExposed));
    }

    return result;
}


Array<WeakReference<ControllableContainer > > ControllableContainer::getAllControllableContainers (bool recursive)
{
    Array<WeakReference<ControllableContainer>> containers;
    containers.addArray (controllableContainers);

    if (!recursive)
    {
        return containers;
    }

    {
        ScopedLock lk (controllableContainers.getLock());

        for (auto& cc : controllableContainers) if (cc.get())containers.addArray (cc->getAllControllableContainers (true));

    }

    return containers;

}



Controllable* ControllableContainer::getControllableForAddress (String address, bool getNotExposed)const
{
    StringArray addrArray;
    addrArray.addTokens (address.toLowerCase(), juce::StringRef ("/"), juce::StringRef ("\""));

    // remove first when address starts with " / "
    if(addrArray.size() && addrArray.getReference(0).isEmpty())
        addrArray.remove (0);

    ControlAddressType addr;
    for(auto & s:addrArray){
        addr.add(s);
    }

    return getControllableForAddress (addr, getNotExposed);
}


Controllable* ControllableContainer::getControllableForAddress (ControlAddressType & addressSplit, bool getNotExposed)const
{
    if (addressSplit.size() == 0) jassertfalse; // SHOULD NEVER BE THERE !
    return addressSplit.resolveControllableFromContainer(this);
}

ControllableContainer* ControllableContainer::getControllableContainerForAddress (ControlAddressType & a, bool getNotExposed )const{
    if (a.size() == 0) jassertfalse; // SHOULD NEVER BE THERE !
    return a.resolveContainerFromContainer(this);
}

Array<Controllable*> ControllableContainer::getControllablesForExtendedAddress (StringArray addressSplit, bool recursive, bool getNotExposed)const
{
    if (addressSplit.size() == 0) jassertfalse; // SHOULD NEVER BE THERE !
    Array<Controllable *> res;
    bool isTargetAControllable = addressSplit.size() == 1;

    if (isTargetAControllable)
    {
        {
            //DBG("Check controllable Address : " + shortName);
            const ScopedLock lk (controllables.getLock());

            for (const auto& c : controllables)
            {
                if (c->isControllableExposed || getNotExposed){
                    OSCAddress ad("/"+c->shortName);
                    OSCAddressPattern pat ("/"+addressSplit[0]);
                    if ( pat.matches(ad))
                    {
                        //DBG(c->shortName);
                        res.add(c);

                    }
                }
            }
        }

    }

    else
    {
        ScopedLock lk (controllableContainers.getLock());
        auto deeperAddr = addressSplit;
        deeperAddr.remove(0);

        for (auto& cc : controllableContainers)
        {

            if (cc.get())
            {

                OSCAddress ad("/"+cc->shortName);
                OSCAddressPattern pat ("/"+addressSplit[0]);
                if (pat.matches(ad))
                {

                    res.addArray(cc->getControllablesForExtendedAddress (deeperAddr, recursive, getNotExposed));
                }


            }
        }
    }

    return res;
}


bool ControllableContainer::containsControllable (const Controllable* c, int maxSearchLevels)const
{
    if (c == nullptr) return false;

    const ControllableContainer* pc = c->parentContainer;

    if (pc == nullptr) return false;

    int curLevel = 0;

    while (pc != nullptr)
    {
        if (pc == this) return true;

        curLevel++;

        if (maxSearchLevels >= 0 && curLevel > maxSearchLevels) return false;

        pc = pc->parentContainer;
    }

    return false;
}



void ControllableContainer::dispatchFeedback (Controllable* c)
{

    if (parentContainer != nullptr) { parentContainer->dispatchFeedback (c); }

    controllableContainerFBListeners.call (&FeedbackListener::controllableFeedbackUpdate, this, c);

}




String ControllableContainer::getUniqueNameInContainer (const String& sourceName, int suffix, void* me)
{
    String resultName = sourceName;

    if (suffix > 0)
    {
        StringArray sa;
        sa.addTokens (resultName, false);

        if (sa.size() > 1 && (sa[sa.size() - 1].getIntValue() != 0 || sa[sa.size() - 1].containsOnly ("0")))
        {
            int num = sa[sa.size() - 1].getIntValue() + suffix;
            sa.remove (sa.size() - 1);
            sa.add (String (num));
            resultName = sa.joinIntoString (" ");
        }
        else
        {
            resultName += " " + String (suffix);
        }
    }

    ShortNameType tShort = Controllable::toShortName(resultName);
    
    void* elem = getControllableByShortName (tShort);

    if ( elem != nullptr && elem != me)
    {
        return getUniqueNameInContainer (sourceName, suffix + 1, me);
    }
    
    elem = getControllableContainerByShortName (tShort) ;
    
    if (elem != nullptr && elem != me)
    {
        return getUniqueNameInContainer (sourceName, suffix + 1, me);
    }
    
    return resultName;
}



bool ControllableContainer::containsContainer (ControllableContainer* c) const
{
    if (c == this)return true;
    
    ScopedLock lk (controllableContainers.getLock());
    
    for (auto& cc : controllableContainers)
    {
        if (c == cc) {return true;}
        
        if (cc->containsContainer (c))return true;
    }
    
    return false;
}


void ControllableContainer::setUserDefined (bool v)
{
    isUserDefined = v;
    
}

void ControllableContainer::addControllableContainerListener (Listener* newListener) {
    controllableContainerListeners.add (newListener);
    newListener->listenedContainers.addIfNotAlreadyThere(this);
}
void ControllableContainer::removeControllableContainerListener (Listener* listener) {
    controllableContainerListeners.remove (listener);
    listener->listenedContainers.removeAllInstancesOf(this);
}
void ControllableContainer::addControllableContainerListener (FeedbackListener* newListener) {
    addControllableContainerListener ((Listener*)newListener);
    controllableContainerFBListeners.add (newListener);
    newListener->listenedFBContainers.addIfNotAlreadyThere(this);
}
void ControllableContainer::removeControllableContainerListener (FeedbackListener* listener) {
    removeControllableContainerListener((Listener*)listener);
    controllableContainerFBListeners.remove (listener);
    listener->listenedFBContainers.removeAllInstancesOf(this);
}

