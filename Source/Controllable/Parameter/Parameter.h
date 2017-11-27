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


class Parameter : public Controllable, private AsyncUpdater
{
public:
    Parameter ( const String& niceName, const String& description, var initialValue, bool enabled = true);

    virtual ~Parameter() {Parameter::masterReference.clear(); cancelPendingUpdate();}



    var defaultValue;
    var value;
    var lastValue;


    virtual bool isMappable() override;

    bool isEditable;
    bool alwaysNotify; // force notifying even if not changed

    bool isPresettable;
    bool isOverriden;
    bool mappingDisabled;
    // if true each set value doesn't do nothing until som reader call pushValue
    // useful for thread syncronization
    bool isCommitableParameter;

    // when race condition are met, do we lock?
    bool isLocking;
    volatile bool isSettingValue;

    void setNewDefault(const var & value,bool notify);
    void resetValue (bool silentSet = false,bool force = false);
    void setValue (const var & _value, bool silentSet = false, bool force = false);
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
    float floatValue() const { return (float)value; }
    double doubleValue() const {return (double)value;}
    int intValue() const { return (int)value; }
    bool boolValue() const { return (bool)value; }
    virtual String stringValue() const { return value.toString(); }

    void notifyValueChanged (bool defferIt = false);

    virtual DynamicObject* createDynamicObject() override;


    //Listener
    class  Listener
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
        virtual void parameterValueChanged (Parameter* p) = 0;
        virtual void parameterRangeChanged (Parameter* ) {};
        Array<WeakReference<Parameter> > linkedP;
    };



    ListenerList<Listener> listeners;
    void addParameterListener (Listener* newListener) {
        listeners.add (newListener);
        newListener->linkedP.add(this);
    }
    void removeParameterListener (Listener* listener) {
        listeners.remove (listener);
        listener->linkedP.removeAllInstancesOf(this);
    }



    // ASYNC
    class  ParamWithValue
    {
    public:
        ParamWithValue (Parameter* p, const var & v, bool _isRange): parameter (p), value (v), m_isRange (_isRange) {}
        Parameter* parameter;
        var value;
        bool m_isRange;
        bool isRange() const {return m_isRange;}

    };

    typedef QueuedNotifier<ParamWithValue>::Listener AsyncListener;
    QueuedNotifier<ParamWithValue> queuedNotifier;
    void handleAsyncUpdate()override;


    void addAsyncParameterListener (AsyncListener* newListener) { queuedNotifier.addListener (newListener); }
    void addAsyncCoalescedListener (AsyncListener* newListener) { queuedNotifier.addAsyncCoalescedListener (newListener); }
    void removeAsyncParameterListener (AsyncListener* listener) { queuedNotifier.removeListener (listener); }


    //JS Helper
    virtual DynamicObject* getObject() override;
    virtual var getVarState() override;


    virtual void tryToSetValue (const var & _value, bool silentSet, bool force );

    static const Identifier valueIdentifier;


    static Parameter* fromControllable (Controllable* c) {return static_cast<Parameter*> (c);}

    template<typename T> T* getAs() {return dynamic_cast<T*> (this);}


protected:
    bool waitOrDeffer (const var& _value, bool silentSet, bool force);

private:




    void checkVarIsConsistentWithType();

    WeakReference<Parameter>::Master masterReference;
    friend class WeakReference<Parameter>;


    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (Parameter)

};






#endif  // PARAMETER_H_INCLUDED
