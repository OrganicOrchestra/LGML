/* Copyright © Organic Orchestra, 2017
*
* This file is part of LGML.  LGML is a software to manipulate sound in realtime
*
* This program is free software; you can redistribute it and/or modify
* it under the terms of the GNU General Public License as published by
* the Free Software Foundation (version 3 of the License).
*
* This program is distributed in the hope that it will be useful,
* but WITHOUT ANY WARRANTY; without even the implied warranty of
* MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
*
*/


#include "SerialUIHelper.h"

SerialDeviceChooser::SerialDeviceChooser()
{
    SerialManager::getInstance()->addSerialManagerListener (this);
    fillDeviceList();
}

SerialDeviceChooser::~SerialDeviceChooser()
{
    SerialManager::getInstance()->removeSerialManagerListener (this);
}

void SerialDeviceChooser::fillDeviceList()
{
    String currentSelected = getItemText (getSelectedItemIndex());

    clear();
    addItem ("Choose a Serial Device", 1);
    int menuIndex = 2;

    int newSelectedId = 1;

    for (auto& pi : SerialManager::getInstance()->portInfos)
    {
        String mName = pi->description + String ("(") + pi->port + String (")");

        if (mName == currentSelected) newSelectedId = menuIndex;

        addItem (mName, menuIndex);
        menuIndex++;
    }

    setSelectedId (newSelectedId, NotificationType::dontSendNotification);

}

void SerialDeviceChooser::portAdded (SerialPortInfo*)
{
    fillDeviceList();

}

void SerialDeviceChooser::portRemoved (SerialPortInfo*)
{
    fillDeviceList();
}
