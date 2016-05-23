/*
  ==============================================================================

    SerialUIHelper.cpp
    Created: 22 May 2016 6:35:02pm
    Author:  bkupe

  ==============================================================================
*/

#include "SerialUIHelper.h"

SerialDeviceChooser::SerialDeviceChooser()
{
	SerialManager::getInstance()->addSerialManagerListener(this);
	fillDeviceList();
}

SerialDeviceChooser::~SerialDeviceChooser()
{
	SerialManager::getInstance()->removeSerialManagerListener(this);
}

void SerialDeviceChooser::fillDeviceList()
{
	String currentSelected = getItemText(getSelectedItemIndex());

	clear();
	addItem("Choose a Serial Device", 1);
	int menuIndex = 2;

	int newSelectedId = 1;

	for (auto &pi : SerialManager::getInstance()->portInfos)
	{
		if (pi->description == currentSelected) newSelectedId = menuIndex;
		addItem(pi->description,menuIndex);
		menuIndex++;
	}

	setSelectedId(newSelectedId, NotificationType::dontSendNotification);

}

void SerialDeviceChooser::portAdded(SerialPortInfo *)
{
	fillDeviceList();

}

void SerialDeviceChooser::portRemoved(SerialPortInfo *)
{
	fillDeviceList();
}
