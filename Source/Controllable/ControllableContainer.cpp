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

#include "ControllableContainer.h"


#include "../Utils/DebugHelpers.h"
#include "../Utils/StringUtil.hpp"
#include "../Scripting/Js/JsHelpers.h"
#include "Parameter/ParameterFactory.h"





const Identifier ControllableContainer::controlAddressIdentifier ("controlAddress");

const Identifier ControllableContainer::childContainerId ("/");

const Identifier ControllableContainer::controllablesId ("parameters");


ControllableContainer::ControllableContainer (StringRef niceName) :
    parentContainer (nullptr),
    hasCustomShortName (false),
    skipControllableNameInAddress (false),
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
    masterReference.clear();
}


void ControllableContainer::clearContainer()
{

    while (controllables.size())
    {
        removeControllable (controllables[0]);
    }

    controllableContainers.clear();
    controllableContainerListeners.call (&Listener::containerCleared, this);
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
    controllableContainerListeners.call (&Listener::controllableRemoved, this, c);


    controllables.removeObject (c);
    notifyStructureChanged (this);
}


void ControllableContainer::notifyStructureChanged (ControllableContainer* origin)
{

    controllableContainerListeners.call (&Listener::childStructureChanged, this, origin);

    if (parentContainer)
    {
        parentContainer->notifyStructureChanged (origin);
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


void ControllableContainer::setCustomShortName (const String& _shortName)
{
    shortName = _shortName;
    hasCustomShortName = true;
    updateChildrenControlAddress();
    controllableContainerListeners.call (&Listener::childAddressChanged, this);
}


void ControllableContainer::setAutoShortName()
{
    hasCustomShortName = false;
    shortName = StringUtil::toShortName (getNiceName());
    updateChildrenControlAddress();
    controllableContainerListeners.call (&Listener::childAddressChanged, this);
}



Controllable* ControllableContainer::getControllableByName (const String& name, bool searchNiceNameToo)
{
    ScopedLock lk (controllables.getLock());

    for (auto& c : controllables)
    {
        if (c->shortName == name || (searchNiceNameToo && c->niceName == name)) return c;
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
    container->setParentContainer (this);

    if (notify)
    {
        controllableContainerListeners.call (&Listener::controllableContainerAdded, this, container);
        notifyStructureChanged (this);
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

    //  container->removeControllableContainerListener(this);
    notifyStructureChanged (this);
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
    notifyStructureChanged (this);
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

ControllableContainer* ControllableContainer::getControllableContainerByName (const String& name, bool searchNiceNameToo)
{
    ScopedLock lk (controllableContainers.getLock());

    for (auto& cc : controllableContainers)
    {
        if (cc.get() && (cc->shortName == name || (searchNiceNameToo && cc->getNiceName() == name))) return cc;
    }

    return nullptr;

}


ControllableContainer* ControllableContainer::getControllableContainerForAddress ( StringArray  addressSplit)
{

    if (addressSplit.size() == 0) jassertfalse; // SHOULD NEVER BE THERE !

    bool isTargetAControllable = addressSplit.size() == 1;

    if (isTargetAControllable)
    {

        if (ControllableContainer* res = getControllableContainerByName (addressSplit[0]))
            return res;

        //no found in direct children Container, maybe in a skip container ?
        ScopedLock lk (controllableContainers.getLock());

        for (auto& cc : controllableContainers)
        {
            if (cc->skipControllableNameInAddress)
            {
                if (ControllableContainer* res = cc->getControllableContainerForAddress (addressSplit)) return res;
            }
        }
    }
    else
    {
        ScopedLock lk (controllableContainers.getLock());

        for (auto& cc : controllableContainers)
        {

            if (!cc->skipControllableNameInAddress)
            {
                if (cc->shortName == addressSplit[0])
                {
                    addressSplit.remove (0);
                    return cc->getControllableContainerForAddress (addressSplit);
                }
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


String ControllableContainer::getControlAddress (ControllableContainer* relativeTo)
{
    StringArray addressArray;
    ControllableContainer* pc = this;

    while (pc != relativeTo && pc != nullptr)
    {
        if (!pc->skipControllableNameInAddress) addressArray.insert (0, pc->shortName);

        pc = pc->parentContainer;
    }

    if (addressArray.size() == 0)return "";
    else return "/" + addressArray.joinIntoString ("/");
}


void ControllableContainer::setParentContainer (ControllableContainer* container)
{
    this->parentContainer = container;
    updateChildrenControlAddress();

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

        for (auto& c : controllables)
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
    addrArray.addTokens (address, juce::StringRef ("/"), juce::StringRef ("\""));
    addrArray.remove (0);

    return getControllableForAddress (addrArray, recursive, getNotExposed);
}


Controllable* ControllableContainer::getControllableForAddress (StringArray addressSplit, bool recursive, bool getNotExposed)
{
    if (addressSplit.size() == 0) jassertfalse; // SHOULD NEVER BE THERE !

    bool isTargetAControllable = addressSplit.size() == 1;

    if (isTargetAControllable)
    {
        {
            //DBG("Check controllable Address : " + shortName);
            const ScopedLock lk (controllables.getLock());

            for (auto& c : controllables)
            {
                if (c->shortName == addressSplit[0])
                {
                    //DBG(c->shortName);
                    if (c->isControllableExposed || getNotExposed) return c;
                    else return nullptr;
                }
            }
        }
        {
            //no found in direct children controllables, maybe in a skip container ?
            ScopedLock lk (controllableContainers.getLock());

            for (auto& cc : controllableContainers)
            {
                if (cc.get())
                {
                    if (cc->skipControllableNameInAddress)
                    {
                        Controllable* tc = cc->getControllableByName (addressSplit[0]);

                        if (tc != nullptr) return tc;
                    }
                }
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
                if (!cc->skipControllableNameInAddress)
                {
                    if (cc->shortName == addressSplit[0])
                    {
                        addressSplit.remove (0);
                        return cc->getControllableForAddress (addressSplit, recursive, getNotExposed);
                    }
                }
                else
                {
                    ControllableContainer* tc = cc->getControllableContainerByName (addressSplit[0]);

                    if (tc != nullptr)
                    {
                        addressSplit.remove (0);
                        return tc->getControllableForAddress (addressSplit, recursive, getNotExposed);
                    }

                }
            }
        }
    }

    return nullptr;
}


bool ControllableContainer::containsControllable (Controllable* c, int maxSearchLevels)
{
    if (c == nullptr) return false;

    ControllableContainer* pc = c->parentContainer;

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

    controllableContainerListeners.call (&Listener::controllableFeedbackUpdate, this, c);

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
