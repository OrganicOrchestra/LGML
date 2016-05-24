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
	String currentSelected = getItemText(getSelectedItemIndex());

	clear();
	addItem("Choose a MIDI Device", 1);
	StringArray deviceList = isInputChooser ? MIDIManager::getInstance()->inputDevices : MIDIManager::getInstance()->outputDevices;
	addItemList(deviceList, 2);
	if (!currentSelected.isEmpty())
	{
		setSelectedItemIndex(deviceList.indexOf(currentSelected) + 2,NotificationType::dontSendNotification); //Start with id 1
	}
	else
	{
		setSelectedId(1, NotificationType::dontSendNotification);
	}

}

void MIDIDeviceChooser::setSelectedDevice(const String & deviceName, bool silent)
{
	StringArray deviceList = isInputChooser ? MIDIManager::getInstance()->inputDevices : MIDIManager::getInstance()->outputDevices;
	int dIndex = deviceList.indexOf(deviceName);
	if (dIndex == -1) return;
	setSelectedItemIndex(dIndex+1,silent?NotificationType::dontSendNotification:NotificationType::sendNotification);
}

void MIDIDeviceChooser::midiInputAdded(String &)
{
	if (!isInputChooser) return;
	fillDeviceList();
}

void MIDIDeviceChooser::midiInputRemoved(String &)
{
	if (!isInputChooser) return;
	fillDeviceList();
}

void MIDIDeviceChooser::midiOutputAdded(String &)
{
	if (isInputChooser) return;
	fillDeviceList();
}

void MIDIDeviceChooser::midiOutputRemoved(String &)
{
	if (isInputChooser) return;
	fillDeviceList();
}
