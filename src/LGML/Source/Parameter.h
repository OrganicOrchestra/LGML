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
    Parameter(const Type &type, const String & niceName, const String &description, bool enabled = true);
    virtual ~Parameter() {Parameter::masterReference.clear();}

    virtual float getNormalizedValue() {
        return 0;
    }

    virtual String toString() = 0;
    virtual void fromString(const String & s,bool silentSet = false, bool force = false) = 0;
protected:
    void notifyValueChanged() {listeners.call(&Listener::parameterValueChanged, this); }


public:
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



private:

    WeakReference<Parameter>::Master masterReference;
    friend class WeakReference<Parameter>;


    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(Parameter)

};


#endif  // PARAMETER_H_INCLUDED
