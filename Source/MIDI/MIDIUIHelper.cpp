/*
 ==============================================================================

 Copyright © Organic Orchestra, 2017

 This file is part of LGML. LGML is a software to manipulate sound in realtime

 This program is free software; you can redistribute it and/or modify
 it under the terms of the GNU General Public License as published by
 the Free Software Foundation (version 3 of the License).

 This program is distributed in the hope that it will be useful,
 but WITHOUT ANY WARRANTY; without even the implied warranty of
 MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.

 ==============================================================================
 */

#include "MIDIUIHelper.h"

MIDIDeviceChooser::MIDIDeviceChooser (bool _isInputChooser) :
    isInputChooser (_isInputChooser)
{
    MIDIManager::getInstance()->addMIDIListener (this);
    fillDeviceList();
}

MIDIDeviceChooser::~MIDIDeviceChooser()
{
    MIDIManager::getInstance()->removeMIDIListener (this);
}

void MIDIDeviceChooser::fillDeviceList()
{
    String currentSelected = currentDeviceName;// getSelectedId() > 1 ? getItemText(getSelectedItemIndex()) : String::empty;


    clear();
    addItem ("Choose a MIDI Device", 1);
    StringArray deviceList = isInputChooser ? MIDIManager::getInstance()->inputDevices : MIDIManager::getInstance()->outputDevices;
    addItemList (deviceList, 2);

    setSelectedDevice (currentSelected, true);

}

void MIDIDeviceChooser::setSelectedDevice (const String& deviceName, bool silent)
{
    currentDeviceName = deviceName;

    if (deviceName.isEmpty())
    {
        setSelectedId (1, NotificationType::dontSendNotification);
        return;
    }

    StringArray deviceList = isInputChooser ? MIDIManager::getInstance()->inputDevices : MIDIManager::getInstance()->outputDevices;
    int dIndex = deviceList.indexOf (deviceName);

    if (dIndex == -1)
    {
        setSelectedId (1, NotificationType::dontSendNotification);
    }
    else
    {
        setSelectedItemIndex (dIndex + 1, silent ? NotificationType::dontSendNotification : NotificationType::sendNotification);
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
