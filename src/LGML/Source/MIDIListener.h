/*
  ==============================================================================

    MIDIListener.h
    Created: 12 May 2016 9:42:56am
    Author:  Martin Hermant

  ==============================================================================
*/

#ifndef MIDILISTENER_H_INCLUDED
#define MIDILISTENER_H_INCLUDED
#pragma once
#include "MIDIManager.h"

// abstract base class that provide midi callback handle and keeps MIDIManager in sync

class MIDIListener :public MidiInputCallback,MIDIManager::MIDIManagerListener
{
    public :
    MIDIListener();
    virtual ~MIDIListener();


    String midiPortName;
	String ghostPortName;

    virtual void setCurrentDevice(const String &deviceName);
	
	//Output
	ScopedPointer<MidiOutput> midiOutDevice;
	void sendNoteOn(int channel, int pitch, int velocity);
	void sendNoteOff(int channel, int pitch, int velocity);
	void sendCC(int channel, int number, int value);
	void sendSysEx(uint8 * data, int dataCount);

	virtual void midiInputAdded(String & s) override;
	virtual void midiInputRemoved(String & s) override;
//    virtual void midiInputsChanged() {}
    
	virtual void midiOutputAdded(String & s) override;
	virtual void midiOutputRemoved(String & s) override;
//    virtual void midiOutputsChanged() {}
    
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
