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
	void fillDeviceList();

	void setSelectedDevice(const String &deviceName, bool silent = true);

	// Inherited via MIDIManagerListener
	/*
	virtual void midiInputAdded(String & inputName) override;
	virtual void midiInputRemoved(String & inputName) override;
	virtual void midiOutputAdded(String & outputName) override;
	virtual void midiOutputRemoved(String & outputName) override;
	*/

	virtual void midiInputsChanged() override;
	virtual void midiOutputsChanged() override;
};

#endif  // MIDIUIHELPER_H_INCLUDED
