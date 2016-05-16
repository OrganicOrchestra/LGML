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
#include "QueuedNotifier.h"

class Parameter : public Controllable
{
public:
    Parameter(const Type &type, const String & niceName, const String &description, var initialValue, var minValue, var maxValue, bool enabled = true);
    virtual ~Parameter() {Parameter::masterReference.clear();}


    var defaultValue;
    var value;
    var minimumValue;
    var maximumValue;
    void setRange(var,var);

	bool isEditable;

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
        virtual void parameterRangeChanged(Parameter * ){};
    };

    ListenerList<Listener> listeners;
    void addParameterListener(Listener* newListener) { listeners.add(newListener); }
    void removeParameterListener(Listener* listener) { listeners.remove(listener); }



    // ASYNC
    class  ParamWithValue{
    public:
        ParamWithValue(Parameter * p,var v):parameter(p),value(v){}
        Parameter * parameter;
        var value;

    };
    QueuedNotifier<ParamWithValue> queuedNotifier;
    typedef QueuedNotifier<ParamWithValue>::Listener AsyncListener;


    void addAsyncParameterListener(AsyncListener* newListener) { queuedNotifier.addListener(newListener); }
    void addAsyncCoalescedListener(AsyncListener* newListener) { queuedNotifier.addAsyncCoalescedListener(newListener); }
    void removeAsyncParameterListener(AsyncListener* listener) { queuedNotifier.removeListener(listener); }

private:





    WeakReference<Parameter>::Master masterReference;
    friend class WeakReference<Parameter>;


    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(Parameter)

};


#endif  // PARAMETER_H_INCLUDED
