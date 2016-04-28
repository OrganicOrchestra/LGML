/*
 ==============================================================================

 Parameter.h
 Created: 8 Mar 2016 1:08:19pm
 Author:  bkupe

 ==============================================================================
 */

#ifndef PARAMETER_H_INCLUDED
#define PARAMETER_H_INCLUDED

#include "Controllable.h"

class Parameter : public Controllable
{
public:
    Parameter(const Type &type, const String & niceName, const String &description, var initialValue, var minValue, var maxValue, bool enabled = true);
    virtual ~Parameter() {Parameter::masterReference.clear();}


    var defaultValue;
    var minimumValue;
    var maximumValue;
    var value;

    bool isPresettable;
    bool isOverriden;

    void resetValue();
    void setValue(var _value, bool silentSet = false, bool force = false);
    virtual void setValueInternal(var _value);

    //For Number type parameters
    void setNormalizedValue(const float &normalizedValue, bool silentSet = false, bool force = false);
    float getNormalizedValue();

    //helpers for fast typing
    float floatValue() { return (float)value; }
    int intValue() { return (int)value; }
    bool boolValue() { return (bool)value; }
    String stringValue() { return value.toString(); }

    void notifyValueChanged();

    //Listener
    class  Listener
    {
    public:
        /** Destructor. */
        virtual ~Listener() {}
        virtual void parameterValueChanged(Parameter * p) = 0;
    };
    ListenerList<Listener> listeners;
    void addParameterListener(Listener* newListener) { listeners.add(newListener); }
    void removeParameterListener(Listener* listener) { listeners.remove(listener); }

    class AsyncListener : public AsyncUpdater,public Parameter::Listener{
    public:
        AsyncListener(bool onlyLast = true):getOnlyLastValue(onlyLast){}
        virtual ~AsyncListener() {}

        // allow to stack all values or get oly last updated value
        bool getOnlyLastValue;

        // allow asyncronous calls to be stacked(if !getOnlyLast) then called from the message thread
        virtual void asyncParameterValueChanged(Parameter * p,var & v) = 0;

        void handleAsyncUpdate()override{
            const ScopedLock lk(mu);
            for(auto &v:asyncVars){
                asyncParameterValueChanged(v.parameter,v.value);
            }
            asyncVars.clear();
        }
    private:
        void parameterValueChanged(Parameter * p) override{
            {
                const ScopedLock lk(mu);
                if(getOnlyLastValue){
                    bool found = false;
                    for(auto & av:asyncVars){
                        if(av.parameter==p){
                            av.value = p->value;
                            found = true;
                            break;
                        }
                    }
                    if(!found){asyncVars.add(ParameterWithValue(p,p->value));}
                }
                else{asyncVars.add(ParameterWithValue(p,p->value));}
            }
            triggerAsyncUpdate();
        };

        friend class Parameter;
        struct ParameterWithValue{
            ParameterWithValue(Parameter * p,var v):parameter(p),value(v){}
            void set(Parameter * p,var v){parameter=p;value =v;}
            Parameter * parameter;
            var value;
        };
        Array<ParameterWithValue> asyncVars;
        CriticalSection mu;

    };

    
    ListenerList<AsyncListener> asyncListeners;
    void addParameterListener(AsyncListener* newListener) { asyncListeners.add(newListener); }
    void removeParameterListener(AsyncListener* listener) { asyncListeners.remove(listener); }

private:

    WeakReference<Parameter>::Master masterReference;
    friend class WeakReference<Parameter>;


    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(Parameter)
    
};


#endif  // PARAMETER_H_INCLUDED
