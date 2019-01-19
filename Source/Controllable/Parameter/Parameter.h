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


#ifndef PARAMETER_H_INCLUDED
#define PARAMETER_H_INCLUDED

#include "../Controllable.h"
#include "../../Utils/QueuedNotifier.h"

#if USE_FLOAT_FOR_PARAMS
typedef float floatParamType;
#else
typedef double floatParamType;
#endif

class ParameterBase : public Controllable, private AsyncUpdater
{
public:
    class Listener;
    ParameterBase ( const String& niceName, const String& description, var initialValue, bool enabled = true);

    virtual ~ParameterBase() ;



    var defaultValue;
    var value;
    var lastValue;


    virtual bool isMappable() override;

    void setInternalOnlyFlags(bool isVisible=true,bool _isSavable=false);// shortcut for internalParameters
    void setSavable(bool savable);
    bool isEditable;
    bool alwaysNotify; // force notifying even if not changed

    
    bool isOverriden;
    bool mappingDisabled;

    // if true each set value doesn't do nothing until som reader call pushValue
    // useful for thread syncronization
    bool isCommitableParameter;

    // when race conditions are met, do we lock?
    bool isLocking;
    bool isSettingValue();


    void setNewDefault(const var & value,bool notify);
    void resetValue (bool silentSet = false,bool force = false);
    void setValue (const var & _value, bool silentSet = false, bool force = false);
    void setValueFrom(Listener * notifier,const var & _value, bool silentSet = false, bool force = false);
    void configureFromObject (DynamicObject*) override;
    void setStateFromVar (const var&) override;

    // helpers to coalesce value until a reader pushes it
    // useful for threadSyncronization
    virtual void commitValue (var _value);
    virtual void pushValue (bool force = false);

    var commitedValue;
    volatile bool hasCommitedValue;

    virtual void setValueInternal (const var& _value);

    virtual bool checkValueIsTheSame (const var& v1, const var& v2); //can be overriden to modify check behavior


    //helpers for fast typing
    float floatValue() const ;
    double doubleValue() const ;
    int intValue() const ;
    bool boolValue() const ;
    virtual String stringValue() const ;



    virtual DynamicObject* createDynamicObject() override;


    // ASYNC
    class  ParamWithValue
    {
    public:
        ParamWithValue ( ParameterBase* p, const var & v, bool _isRange,Listener* _notifier=nullptr): parameter (p), value (v), m_isRange (_isRange),notifier(_notifier) {}
        ParameterBase* parameter;
        var value;
        bool m_isRange;
        Listener* notifier;
        bool isRange() const {return m_isRange;}

    };
    //Listener
    class  Listener : public QueuedNotifier<ParamWithValue>::Listener
    {
    public:
        /** Destructor. */
        virtual ~Listener() {
            while(linkedP.size()){
                if(auto p = linkedP.getLast().get())
                    p->removeParameterListener(this);
                else
                    linkedP.removeLast();
            }
        }
        virtual void parameterValueChanged ( ParameterBase* p,ParameterBase::Listener * notifier=nullptr) {};
        virtual void parameterRangeChanged ( ParameterBase* ) {};
        virtual void newMessage (const ParamWithValue& ) override {};

        Array<WeakReference<ParameterBase> > linkedP;
    };



    ListenerList<Listener> listeners;
    void addParameterListener (Listener* newListener) ;
    void removeParameterListener (Listener* listener) ;






//    typedef QueuedNotifier<ParamWithValue>::Listener AsyncListener;
    QueuedNotifier<ParamWithValue> queuedNotifier;
    void handleAsyncUpdate()override;

    void notifyValueChanged (bool defferIt = false,Listener * notifier=nullptr);

    void addAsyncParameterListener (Listener* newListener) ;
    void addAsyncCoalescedListener (Listener* newListener) ;
    void removeAsyncParameterListener (Listener* listener) ;


    //JS Helper
    virtual DynamicObject* createObject() override;
    virtual var getVarState() override;


    virtual void tryToSetValue (const var & _value, bool silentSet, bool force, Listener * notifier=nullptr);

    static const Identifier valueIdentifier;


    static const ParameterBase* fromControllable (const Controllable* c) ;
    static ParameterBase* fromControllable (Controllable* c) ;
    template<typename T> T* getAs() {return dynamic_cast<T*> (this);}



protected:
    bool waitOrDeffer (const var& _value, bool silentSet, bool force);
    Atomic<bool> _isSettingValue;
    Atomic<ParameterBase::Listener * > _valueSetter;
    
private:




    void checkVarIsConsistentWithType();

    WeakReference<ParameterBase>::Master masterReference;
    friend class WeakReference<ParameterBase>;
    
    
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (ParameterBase)
    
};







#endif  // PARAMETER_H_INCLUDED
