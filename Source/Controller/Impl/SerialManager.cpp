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


#include "SerialManager.h"

juce_ImplementSingleton (SerialManager)

SerialManager::SerialManager()
{

}

SerialManager::~SerialManager()
{
    stopTimer();
}

void SerialManager::init()
{
    startTimer (1000);
}

void SerialManager::updateDeviceList()
{
#if SERIALSUPPORT
    std::vector<serial::PortInfo> devices_found = serial::list_ports();
    std::vector<serial::PortInfo>::iterator iter = devices_found.begin();

    OwnedArray<SerialPortInfo> newInfos;

    Array<SerialPortInfo*> portsToNotifyAdded;
    Array<SerialPortInfo*> portsToNotifyRemoved;

    while (iter != devices_found.end())
    {
        serial::PortInfo device = *iter++;
        newInfos.add (new SerialPortInfo (device.port, device.description, device.hardware_id));
    }

    //check added devices
    for (auto& newD : newInfos)
    {
        bool found = false;

        for (auto& sourceD : portInfos)
        {
            if (sourceD->hardwareID == newD->hardwareID && sourceD->port == newD->port)
            {
                found = true;
                break;
            }
        }

        if (!found)
        {
            portsToNotifyAdded.add (newD);
        }
    }

    //check removed
    for (auto& sourceD : portInfos)
    {
        bool found = false;

        for (auto& newD : newInfos)
        {
            if (sourceD->hardwareID == newD->hardwareID && sourceD->port == newD->port)
            {
                found = true;
                break;
            }
        }

        if (!found)
        {
            //create one so portInfos can delete the original
            portsToNotifyRemoved.add (sourceD);
        }
    }

    //check removed devices


    for (auto& p : portsToNotifyRemoved)
    {
        DBG ("Port Removed " << p->description);
        removeOption ( p->port ,true);

        portInfos.removeObject (p, false);
        listeners.call (&SerialManagerListener::portRemoved, p);

        SerialPort* port = getPort (p, false);

        if (port != nullptr) removePort (port);

        delete p;
    }

    for (auto& p : portsToNotifyAdded)
    {
        addOption( p->port , p->hardwareID,true);
        newInfos.removeObject (p, false);
        portInfos.add (p);
        listeners.call (&SerialManagerListener::portAdded, p);

    }

#endif
}

SerialPort* SerialManager::getPort (SerialPortInfo* portInfo, bool createIfNotThere, int openBaudRate)
{
#if SERIALSUPPORT

    for (auto& sp : openedPorts)
    {

        if (sp->info->hardwareID == portInfo->hardwareID && sp->info->port == portInfo->port) return sp;
    }

    if (createIfNotThere)
    {
        Serial* newSerial = new Serial (portInfo->port.toStdString(), openBaudRate, serial::Timeout::simpleTimeout (1000));
        SerialPort* p = new SerialPort (newSerial, portInfo);
        openedPorts.add (p);
        return p;
    }

#endif

    return nullptr;
}

SerialPort* SerialManager::getPort (String hardwareID, String portName, bool createIfNotThere)
{
#if SERIALSUPPORT

    for (auto& pi : portInfos)
    {

        if (pi->hardwareID == hardwareID & pi->port == portName) return getPort (pi, createIfNotThere);
    }

#endif

    return nullptr;
}

void SerialManager::removePort (SerialPort* p)
{
    openedPorts.removeObject (p, true);
}

void SerialManager::timerCallback()
{
    updateDeviceList();
}
