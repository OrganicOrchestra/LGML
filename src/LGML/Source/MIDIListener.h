/*
  ==============================================================================

    MIDIListener.h
    Created: 12 May 2016 9:42:56am
    Author:  Martin Hermant

  ==============================================================================
*/

#ifndef MIDILISTENER_H_INCLUDED
#define MIDILISTENER_H_INCLUDED

#include "JuceHeader.h"

// abstract base class that provide midi callback handle and keeps MIDIManager in sync

class MIDIListener :public MidiInputCallback
{
    public :
    MIDIListener();
    virtual ~MIDIListener(){};

    
    String midiPortName;

    void setCurrentDevice(const String &deviceName);

    // need to implement this from MidiInputCallback
//    virtual void handleIncomingMidiMessage(MidiInput* source,
//                                   const MidiMessage& message) =0;




    class  Listener
    {
    public:
        /** Destructor. */
        virtual ~Listener() {}
        virtual void currentDeviceChanged(MIDIListener *) {}


    };

    ListenerList<Listener> MIDIListenerListeners;
    void addMIDIListenerListener(Listener* newListener) { MIDIListenerListeners.add(newListener); }
    void removeMIDIListenerListener(Listener* listener) { MIDIListenerListeners.remove(listener); }
    
    
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(MIDIListener)
};



#endif  // MIDILISTENER_H_INCLUDED
