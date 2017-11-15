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

#ifndef ControllableCONTAINER_H_INCLUDED
#define ControllableCONTAINER_H_INCLUDED

#include "Controllable.h"


#include "../Utils/DebugHelpers.h"


class ControllableContainer 

{
public:

    ControllableContainer (StringRef niceName);
    virtual ~ControllableContainer();

    void setUserDefined (bool);
    
    void setAutoShortName();


    // variables
    String shortName;
    bool isUserDefined;
    
    
    Uuid uid;
    OwnedArray<Controllable, CriticalSection> controllables;
    Array<WeakReference<ControllableContainer >, CriticalSection  > controllableContainers;
    ControllableContainer* parentContainer;




    // name functions to override
    virtual String const getNiceName() = 0;
    virtual String setNiceName (const String& _niceName);

    void removeFromParent();

    void clearContainer();

    void removeControllable (Controllable* c);

    Controllable* getControllableByName (const String& name, bool searchNiceNameToo = false);

    ControllableContainer* addChildControllableContainer (ControllableContainer* container, bool notify = true);
    ControllableContainer* getRoot(bool getGlobal);
    void removeChildControllableContainer (ControllableContainer* container);
    // add indexed container (ensure localIndex and position in the child container array are the same)
    // idx of -1 add after the ast indexed (may be not the last, array can contain other non indexed elements)
    void addChildIndexedControllableContainer (ControllableContainer* container, int idx = -1);
    void removeChildIndexedControllableContainer (int idx);
    int getNumberOfIndexedContainer();
    int getIndexedPosition();
    bool hasIndexedContainers();
    bool isIndexedContainer();
    // can be overriden if indexed container are removed from the middle of the list,
    // allowing Indexed containers to react to index change
    virtual void localIndexChanged();

    //
    //template


    template<class T>
    Array<WeakReference<T> > getControllablesOfType (bool recursive)
    {
        Array<WeakReference<T> > res;

        for (auto& c : controllables)
        {
            if (T* o = dynamic_cast<T*> (c)) {res.add (o);}
        }

        if (recursive)
        {
            ScopedLock lk (controllableContainers.getLock());

            for (auto& c : controllableContainers) {res.addArray (c->getControllablesOfType<T> (true));}
        }

        return res;
    }

    template<class T>
    Array<T* > getContainersOfType (bool recursive)
    {
        Array<T* > res;
        ScopedLock lk (controllableContainers.getLock());

        for (auto& c : controllableContainers)
        {
            if (c.get())
            {
                if (T* o = dynamic_cast<T*> (c.get())) { res.add (o);}
            }
            else
            {
                jassertfalse;
            }
        }

        if (recursive)
        {
            for (auto& c : controllableContainers)
            {
                if (c.get())
                {
                    res.addArray (c->getContainersOfType<T> (true));
                }
                else
                {
                    jassertfalse;
                }
            }
        }


        return res;
    }


    bool containsContainer (ControllableContainer* );

    ControllableContainer* getControllableContainerByName (const String& name, bool searchNiceNameToo = false);
    ControllableContainer* getControllableContainerForAddress ( StringArray  address);

    void setParentContainer (ControllableContainer* container);
    void updateChildrenControlAddress();


    virtual Array<WeakReference<Controllable>> getAllControllables (bool recursive = false, bool getNotExposed = false);
    virtual Array<WeakReference<ControllableContainer>> getAllControllableContainers (bool recursive = false);

     Controllable* getControllableForAddress (String addressSplit, bool recursive = true, bool getNotExposed = false);
     Controllable* getControllableForAddress (StringArray addressSplit, bool recursive = true, bool getNotExposed = false);
    Array<Controllable*> getControllablesForExtendedAddress (StringArray addressSplit, bool recursive=true, bool getNotExposed=false);
    ControllableContainer * getMirroredContainer(ControllableContainer * other,ControllableContainer * root = nullptr);
    bool containsControllable (Controllable* c, int maxSearchLevels = -1);
    String getControlAddress (ControllableContainer* relativeTo = nullptr);
    StringArray getControlAddressArray (ControllableContainer* relativeTo = nullptr);


    String getUniqueNameInContainer (const String& sourceName, int suffix = 0, void* me = nullptr);

    int numContainerIndexed;
    int localIndexedPosition;

    //Listener
    class  Listener
    {
    public:
        /** Destructor. */
        virtual ~Listener() {}
        virtual void controllableAdded (ControllableContainer*, Controllable*) {}
        virtual void controllableRemoved (ControllableContainer*, Controllable*) {}
        virtual void controllableContainerAdded (ControllableContainer*, ControllableContainer*) {}
        virtual void controllableContainerRemoved (ControllableContainer*, ControllableContainer*) {}
        virtual void controllableFeedbackUpdate (ControllableContainer*, Controllable*) {}
        virtual void childStructureChanged (ControllableContainer* /*notifier*/, ControllableContainer* /*origin*/,bool /*isAdded*/) {}
        virtual void childAddressChanged (ControllableContainer* /*notifier*/,ControllableContainer* ) {};
        virtual void controllableContainerPresetLoaded (ControllableContainer*) {}
        virtual void containerWillClear (ControllableContainer* /*origin*/) {}
    };

    //  typedef ControllableContainerListener Listener ;
    ListenerList<Listener> controllableContainerListeners;
    void addControllableContainerListener (Listener* newListener) { controllableContainerListeners.add (newListener);}
    void removeControllableContainerListener (Listener* listener) { controllableContainerListeners.remove (listener);}


    virtual DynamicObject* getObject() = 0;

    static ControllableContainer * globalRoot;


protected :

    void dispatchFeedback (Controllable* c);

    //  container with custom controllable can override this
    virtual void addControllableInternal (Controllable*) {};

    /// identifiers
    static const Identifier controlAddressIdentifier;
    static const Identifier childContainerId;
    static const Identifier controllablesId;
    friend class PresetManager;

    void notifyStructureChanged (ControllableContainer* origin,bool isAdded);
    void notifyChildAddressChanged (ControllableContainer* origin);

    typename  WeakReference< ControllableContainer >::Master masterReference;
    friend class WeakReference<ControllableContainer>;



    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (ControllableContainer)


};

typedef ControllableContainer::Listener ControllableContainerListener;


#endif  // ControllableCONTAINER_H_INCLUDED
