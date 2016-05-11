/*
  ==============================================================================

    MIDIManager.h
    Created: 11 May 2016 7:17:05pm
    Author:  bkupe

  ==============================================================================
*/

#ifndef MIDIMANAGER_H_INCLUDED
#define MIDIMANAGER_H_INCLUDED

#include "JuceHeader.h"

class MIDIManager : public Timer
{
public:
	juce_DeclareSingleton(MIDIManager,true)

	MIDIManager();
	~MIDIManager();
	
	void init();

	StringArray inputDevices;
	StringArray outputDevices;

	void updateDeviceList(bool updateInput);

	void timerCallback() override;


public:
	class  MIDIManagerListener
	{
	public:
		/** Destructor. */
		virtual ~MIDIManagerListener() {}
		virtual void midiInputAdded(String &inputName) = 0;
		virtual void midiInputRemoved(String &inputName) = 0;
		virtual void midiOutputAdded(String &outputName) = 0;
		virtual void midiOutputRemoved(String &outputName) = 0;


	};

	ListenerList<MIDIManagerListener> listeners;
	void addMIDIListener(MIDIManagerListener* newListener) { listeners.add(newListener); }
	void removeMIDIListener(MIDIManagerListener* listener) { listeners.remove(listener); }



};


#endif  // MIDIMANAGER_H_INCLUDED
