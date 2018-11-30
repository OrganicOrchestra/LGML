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


// TODO fix use of niceName
ControllableContainer::ControllableContainer (StringRef /*niceName*/) :
parentContainer (nullptr),
numContainerIndexed (0),
localIndexedPosition (-1),
isUserDefined (false)
{


}


ControllableContainer::~ControllableContainer()
{
    //controllables.clear();
    //DBG("CLEAR CONTROLLABLE CONTAINER");

    // manage memory if not cleared
    auto all = getAllControllableContainers (true);

    for (auto& a : all)
    {
        if (a.get())delete a.get();
    }


    clearContainer();
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

void ControllableContainer::clearContainer()
{
    controllableContainerListeners.call (&Listener::containerWillClear, this);

    while (controllables.size())
    {
        removeControllable (controllables[0]);
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
    updateChildrenControlAddress();
    notifyChildAddressChanged(this);
}



Controllable* ControllableContainer::getControllableByName (const String& _name,  bool searchNiceName)
{


    ScopedLock lk (controllables.getLock());

    if( searchNiceName){
        for (auto * c : controllables)
        {
            if(c->niceName == _name) {return c;}
        }

    }
    else{
        const String name = Controllable::toShortName(_name);

        for (auto * c : controllables)
        {
            if (c->shortName.compareIgnoreCase(name)==0) return c;
        }
    }
    return nullptr;
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
    jassert(container->parentContainer==nullptr);
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

ControllableContainer* ControllableContainer::getControllableContainerByName (const String& _name, bool searchNiceName)
{

    ScopedLock lk (controllableContainers.getLock());
    if( searchNiceName){
        for (auto & c : controllableContainers)
        {
            if(c->getNiceName() == _name) {return c;}
        }

    }
    else{
        const String name = Controllable::toShortName(_name);

        for (auto& cc : controllableContainers)
        {
            if (cc.get() && (cc->shortName.compareIgnoreCase(name)==0) ) return cc;
        }

    }
    return nullptr;

}


ControllableContainer * ControllableContainer::getMirroredContainer(ControllableContainer * other,ControllableContainer * root ){

    StringArray arr = other->getControlAddressArray(root);
    if(arr.size()==0){
        return this;
    }
    return getControllableContainerForAddress(arr);

}

ControllableContainer* ControllableContainer::getControllableContainerForAddress (StringArray   addressSplit)
{

    if (addressSplit.size() == 0) jassertfalse; // SHOULD NEVER BE THERE !

    bool isTarget = addressSplit.size() == 1;

    if (isTarget)
    {

        if (ControllableContainer* res = getControllableContainerByName (addressSplit[0]))
            return res;

    }
    else
    {
        ScopedLock lk (controllableContainers.getLock());

        for (auto& cc : controllableContainers)
        {
            bool validName = cc->shortName.compareIgnoreCase(addressSplit[0])==0;


            if( validName){
                addressSplit.remove (0);
                return cc->getControllableContainerForAddress (addressSplit);
            }
            else
            {
                ControllableContainer* tc = cc->getControllableContainerByName (addressSplit[0]);

                if (tc != nullptr)
                {
                    addressSplit.remove (0);
                    return tc->getControllableContainerForAddress (addressSplit);
                }

            }
        }
    }

    return nullptr;

}


String ControllableContainer::getControlAddress (const ControllableContainer* relativeTo) const
{
    StringArray addressArray(getControlAddressArray(relativeTo));
    if (addressArray.size() == 0)return "";
    else return "/" + addressArray.joinIntoString ("/");
}

StringArray ControllableContainer::getControlAddressArray (const ControllableContainer* relativeTo) const{
    StringArray addressArray;
    const ControllableContainer* pc = this;

    while (pc != relativeTo && pc->parentContainer != nullptr)
    {
        addressArray.insert (0, pc->shortName);

        pc = pc->parentContainer;
    }
    if(relativeTo){
        jassert(pc == relativeTo);
    }
    return addressArray;
}

void ControllableContainer::setParentContainer (ControllableContainer* container)
{
    this->parentContainer = container;
    if(container!=nullptr)updateChildrenControlAddress();

}


void ControllableContainer::updateChildrenControlAddress()
{
    {
        ScopedLock lk (controllables.getLock());

        for (auto& c : controllables) c->updateControlAddress();
    }
    {
        ScopedLock lk (controllableContainers.getLock());

        for (auto& cc : controllableContainers) if (cc.get())cc->updateChildrenControlAddress();
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



Controllable* ControllableContainer::getControllableForAddress (String address, bool recursive, bool getNotExposed)
{
    StringArray addrArray;
    addrArray.addTokens (address.toLowerCase(), juce::StringRef ("/"), juce::StringRef ("\""));

    // remove first when address starts with " / "
    if(addrArray.size() && addrArray.getReference(0).isEmpty())
        addrArray.remove (0);

    return getControllableForAddress (addrArray, recursive, getNotExposed);
}


Controllable* ControllableContainer::getControllableForAddress (StringArray addressSplit, bool recursive, bool getNotExposed)
{
    if (addressSplit.size() == 0) jassertfalse; // SHOULD NEVER BE THERE !

    bool isTargetAControllable = addressSplit.size() == 1;

    if (isTargetAControllable)
    {

        //DBG("Check controllable Address : " + shortName);
        const ScopedLock lk (controllables.getLock());

        for (const auto& c : controllables)
        {
            if (c->shortName.compareIgnoreCase( addressSplit[0])==0)
            {
                //DBG(c->shortName);
                if (c->isControllableExposed || getNotExposed) return c;
                else return nullptr;
            }
        }
    }

    else
    {
        ScopedLock lk (controllableContainers.getLock());

        for (auto& cc : controllableContainers)
        {
            if (cc.get())
            {

                if (cc->shortName.compareIgnoreCase(addressSplit[0])==0)
                {
                    addressSplit.remove (0);
                    return cc->getControllableForAddress (addressSplit, recursive, getNotExposed);
                }


            }
        }
    }

    return nullptr;
}

Array<Controllable*> ControllableContainer::getControllablesForExtendedAddress (StringArray addressSplit, bool recursive, bool getNotExposed)
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


bool ControllableContainer::containsControllable (const Controllable* c, int maxSearchLevels)
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

    void* elem = getControllableByName (resultName, true);

    if ( elem != nullptr && elem != me)
    {
        return getUniqueNameInContainer (sourceName, suffix + 1, me);
    }
    
    elem = getControllableContainerByName (resultName, true) ;
    
    if (elem != nullptr && elem != me)
    {
        return getUniqueNameInContainer (sourceName, suffix + 1, me);
    }
    
    return resultName;
}



bool ControllableContainer::containsContainer (ControllableContainer* c)
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
