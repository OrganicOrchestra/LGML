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
	struct DeviceUsageCount
	{
	public:
		DeviceUsageCount(const String &name) : deviceName(name), usageCount(0) {}
		String deviceName;
		int usageCount;
	};

	juce_DeclareSingleton(MIDIManager,true)

	MIDIManager();
	~MIDIManager();
	
	void init();

	StringArray inputDevices;
	StringArray outputDevices;

	OwnedArray<DeviceUsageCount> inputCounts;
	OwnedArray<DeviceUsageCount> outputCounts;

	void updateDeviceList(bool updateInput);
	
	void enableInputDevice(const String &deviceName);
	void enableOutputDevice(const String &deviceName);
	void disableInputDevice(const String &deviceName);
	void disableOutputDevice(const String &deviceName);
	
	DeviceUsageCount * getDUCForInputDeviceName(const String &deviceName);
	DeviceUsageCount * getDUCForOutputDeviceName(const String &deviceName);

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
