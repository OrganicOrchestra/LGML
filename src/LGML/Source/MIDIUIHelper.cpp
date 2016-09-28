/*
  ==============================================================================

    MIDIUIHelper.cpp
    Created: 11 May 2016 7:41:09pm
    Author:  bkupe

  ==============================================================================
*/

#include "MIDIUIHelper.h"

MIDIDeviceChooser::MIDIDeviceChooser(bool _isInputChooser) :
	isInputChooser(_isInputChooser)
{
	MIDIManager::getInstance()->addMIDIListener(this);
	fillDeviceList();
}

MIDIDeviceChooser::~MIDIDeviceChooser()
{
	MIDIManager::getInstance()->removeMIDIListener(this);
}

void MIDIDeviceChooser::fillDeviceList()
{
	String currentSelected = currentDeviceName;// getSelectedId() > 1 ? getItemText(getSelectedItemIndex()) : String::empty;

	
	clear();
	addItem("Choose a MIDI Device", 1);
	StringArray deviceList = isInputChooser ? MIDIManager::getInstance()->inputDevices : MIDIManager::getInstance()->outputDevices;
	addItemList(deviceList, 2);

	setSelectedDevice(currentSelected, true);

}

void MIDIDeviceChooser::setSelectedDevice(const String & deviceName, bool silent)
{
	currentDeviceName = deviceName;

	if (deviceName.isEmpty())
	{
		setSelectedId(1, NotificationType::dontSendNotification);
		return;
	}

	StringArray deviceList = isInputChooser ? MIDIManager::getInstance()->inputDevices : MIDIManager::getInstance()->outputDevices;
	int dIndex = deviceList.indexOf(deviceName);

	if (dIndex == -1)
	{
		setSelectedId(1,NotificationType::dontSendNotification);
	} else
	{
		setSelectedItemIndex(dIndex + 1, silent ? NotificationType::dontSendNotification : NotificationType::sendNotification);
	}


}


void MIDIDeviceChooser::midiInputsChanged()
{
	if (!isInputChooser) return;
	fillDeviceList();
}

void MIDIDeviceChooser::midiOutputsChanged()
{
	if (isInputChooser) return;
	fillDeviceList();
}
