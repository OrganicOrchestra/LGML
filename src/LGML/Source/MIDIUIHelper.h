/*
  ==============================================================================

    MIDIUIHelper.h
    Created: 11 May 2016 7:41:09pm
    Author:  bkupe

  ==============================================================================
*/

#ifndef MIDIUIHELPER_H_INCLUDED
#define MIDIUIHELPER_H_INCLUDED

#include "MIDIManager.h"


class MIDIDeviceChooser : public ComboBox, public MIDIManager::MIDIManagerListener
{
public:
	MIDIDeviceChooser(bool isInputChooser);
	virtual ~MIDIDeviceChooser();

	bool isInputChooser;
	
	String currentDeviceName;

	void fillDeviceList();
	void setSelectedDevice(const String &deviceName, bool silent = true);


	virtual void midiInputsChanged() override;
	virtual void midiOutputsChanged() override;
};

#endif  // MIDIUIHELPER_H_INCLUDED
