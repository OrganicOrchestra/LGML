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

#ifndef CONTROLLABLE_H_INCLUDED
#define CONTROLLABLE_H_INCLUDED


#include "../JuceHeaderCore.h"//keep
#include "../Utils/FactoryObject.h"


class ControllableContainer;
class ParameterBase;

typedef Identifier ShortNameType;



class Controllable;
class ControlAddressType : public Array<Identifier>{
public:
    String toString()const;
    static ControlAddressType buildFromControllable(const Controllable * ,const ControllableContainer * maxParent=nullptr);
    static ControlAddressType buildFromControllableContainer(const ControllableContainer * ,const ControllableContainer * maxParent=nullptr);

    Controllable * resolveControllableFromContainer(const ControllableContainer *  c)const;
    ControllableContainer * resolveContainerFromContainer(const ControllableContainer *  c)const;
    ControlAddressType getRelativeTo(ControlAddressType & other)const;
    ControlAddressType subAddr(int start,int end = -1)const;
    StringArray toStringArray()const;
    ControlAddressType getChild(const ShortNameType & c) const;

    static const Identifier rootIdentifier;
};

class Controllable : public FactoryObject
{
public:

    Controllable ( const String& niceName, const String& description, bool enabled = true);
    virtual ~Controllable();



    String niceName;
    ShortNameType shortName;
    String description;


    static ShortNameType toShortName (const String& s);
    
    bool enabled;
    bool isControllableExposed;
    bool isHidenInEditor;
    bool shouldSaveObject;
    bool isUserDefined;
    bool isSavableAsObject;
    bool isSavable;
    ControlAddressType controlAddress;


    WeakReference<ControllableContainer> parentContainer;

    void setNiceName (const String& _niceName);
    
    void setAutoShortName();

    void setEnabled (bool value, bool silentSet = false, bool force = false);

    void setParentContainer (ControllableContainer* container);
    bool isChildOf (const ControllableContainer* p) const;
    void updateControlAddress(bool isParentResolved);

    ControlAddressType getControlAddressRelative (const ControllableContainer* relativeTo = nullptr) const;
    const ControlAddressType & getControlAddress () const;


    virtual bool isMappable();
    bool isPresettable;


    //used for script variables
    virtual DynamicObject* createDynamicObject();
    static var getVarStateFromScript (const juce::var::NativeFunctionArgs& a);


    virtual var getVarState() = 0;
    virtual void setStateFromVar (const var& v) = 0;






public:

    class  Listener
    {
    public:
        /** Destructor. */
        virtual ~Listener() {}
        virtual void controllableStateChanged (Controllable* ) {};
        virtual void controllableControlAddressChanged (Controllable* ) {};
        virtual void controllableNameChanged (Controllable*) {};
        virtual void controllableRemoved (Controllable* ) {};
    };


    ListenerList<Listener> listeners;
    void addControllableListener (Listener* newListener) { listeners.add (newListener); }
    void removeControllableListener (Listener* listener) { listeners.remove (listener); }


    //Script set method handling
    static var setControllableValueFromJS (const juce::var::NativeFunctionArgs& a);


private:

    WeakReference<Controllable >::Master masterReference;
    friend class WeakReference<Controllable >;
    



    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (Controllable)
};

#endif  // CONTROLLABLE_H_INCLUDED
