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

#ifndef ControllableCONTAINER_H_INCLUDED
#define ControllableCONTAINER_H_INCLUDED

#include "Controllable.h"


#include "../Utils/DebugHelpers.h"


class ControllableContainer 

{
public:

    ControllableContainer (StringRef niceName);
    virtual ~ControllableContainer();

    virtual void setUserDefined (bool);
    
    void setAutoShortName();


    // variables
    ShortNameType shortName;
    bool isUserDefined;
    
    
    Uuid uid;
    OwnedArray<Controllable, CriticalSection> controllables;
    Array<WeakReference<ControllableContainer >, CriticalSection  > controllableContainers;
    ControllableContainer* parentContainer;




    // name functions to override
    virtual String const getNiceName() = 0;
    virtual String setNiceName (const String& _niceName)=0;

    void removeFromParent();

    void clearContainer();

    void addControllable(Controllable *c);
    void removeControllable (Controllable* c);

    Controllable* getControllableByName (const String& name) const;
    Controllable* getControllableByShortName(const ShortNameType & n) const;
    Controllable* getControllableByShortName(const String & n) const;
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


    bool containsContainer (ControllableContainer* )const;

    ControllableContainer* getControllableContainerByName (const String& name) const;
    ControllableContainer* getControllableContainerByShortName (const String & name) const;
    ControllableContainer* getControllableContainerByShortName (const ShortNameType & name) const;
    ControllableContainer* findFirstControllableContainer (const std::function<bool(ControllableContainer*)> fun) const;

    void setParentContainer (ControllableContainer* container);
    void updateChildrenControlAddress();


    Array<WeakReference<Controllable>> getAllControllables (bool recursive = false, bool getNotExposed = false);
    Array<WeakReference<ControllableContainer>> getAllControllableContainers (bool recursive = false);

     Controllable* getControllableForAddress (String addressSplit, bool getNotExposed = false) const;
     Controllable* getControllableForAddress (ControlAddressType & a, bool getNotExposed = false)const;
    ControllableContainer* getControllableContainerForAddress (ControlAddressType & a, bool getNotExposed = false)const;
    Array<Controllable*> getControllablesForExtendedAddress (StringArray addressSplit, bool recursive=true, bool getNotExposed=false)const;
    ControllableContainer * getMirroredContainer(ControllableContainer * other,ControllableContainer * root = nullptr);
    bool containsControllable (const Controllable* c, int maxSearchLevels = -1)const;
    ControlAddressType getControlAddressRelative (const ControllableContainer* relativeTo ) const;
    const ControlAddressType & getControlAddress() const;
    ControlAddressType controlAddress;
    


    String getUniqueNameInContainer (const String& sourceName, int suffix = 0, void* me = nullptr); // relates to nice names for efficiency

    int numContainerIndexed;
    int localIndexedPosition;

    //Listener
    class  Listener
    {
    public:
        /** Destructor. */
        virtual ~Listener() {
            while(listenedContainers.size()>0){
                if(auto cc = listenedContainers.getLast().get())
                    cc->removeControllableContainerListener(this);
                else
                    listenedContainers.removeLast();
            }
        }
        virtual void childControllableAdded (ControllableContainer*, Controllable*) {}
        virtual void childControllableRemoved (ControllableContainer*, Controllable*) {}
        virtual void controllableContainerAdded (ControllableContainer*, ControllableContainer*) {}
        virtual void controllableContainerRemoved (ControllableContainer*, ControllableContainer*) {}
        virtual void childStructureChanged (ControllableContainer* /*notifier*/, ControllableContainer* /*origin*/,bool /*isAdded*/) {}
        virtual void childAddressChanged (ControllableContainer* /*notifier*/,ControllableContainer* ) {};
        virtual void containerWillClear (ControllableContainer* /*origin*/) {}
    private:
        friend class ControllableContainer;
        Array<WeakReference<ControllableContainer>> listenedContainers;
    };


    class FeedbackListener : public Listener{
    public:
        virtual ~FeedbackListener(){
            while(listenedFBContainers.size()>0){
                if(auto cc = listenedFBContainers.getLast().get())
                    cc->removeControllableContainerListener(this);
                else
                    listenedFBContainers.removeLast();
            }
        }
        virtual void controllableFeedbackUpdate (ControllableContainer*, Controllable*) =0;
        Array<WeakReference<ControllableContainer>> listenedFBContainers;
    };

    // helper class to inject members
    template<class OwnerClass>
    class OwnedFeedbackListener : public FeedbackListener{
        public:
        OwnedFeedbackListener(OwnerClass * o):owner(o){};
        virtual ~OwnedFeedbackListener(){}
        void controllableFeedbackUpdate (ControllableContainer*, Controllable*) ;
        OwnerClass * owner;

    };
    //  typedef ControllableContainerListener Listener ;
    ListenerList<Listener> controllableContainerListeners;
    ListenerList<FeedbackListener> controllableContainerFBListeners;


    void addControllableContainerListener (Listener* newListener) ;
    void removeControllableContainerListener (Listener* listener) ;
    void addControllableContainerListener (FeedbackListener* newListener) ;
    void removeControllableContainerListener (FeedbackListener* listener) ;


    virtual DynamicObject* createObject() = 0;

    static ControllableContainer * globalRoot;

    

    /// identifiers
    static const Identifier controlAddressIdentifier;
    static const Identifier childContainerId;
    static const Identifier controllablesId;


protected :


    bool canHaveUserDefinedContainers;
    
    void dispatchFeedback (Controllable* c);


    void updateControlAddress(bool isParentResolved = false);
    void notifyStructureChanged (ControllableContainer* origin,bool isAdded,bool controllableUpdated, bool containerUpdated);
    void notifyChildAddressChanged (ControllableContainer* origin);




private:
    WeakReference< ControllableContainer >::Master masterReference;
    friend class WeakReference<ControllableContainer>;
    
    
    
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (ControllableContainer)


};

typedef ControllableContainer::Listener ControllableContainerListener;


#endif  // ControllableCONTAINER_H_INCLUDED
