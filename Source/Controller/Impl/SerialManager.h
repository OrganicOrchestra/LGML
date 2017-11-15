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


#ifndef SERIALMANAGER_H_INCLUDED
#define SERIALMANAGER_H_INCLUDED

#include "SerialPort.h"
#include "../../Controllable/Parameter/EnumParameter.h"



class SerialManager :
    private Timer,
    public EnumParameterModel
{
public:
    juce_DeclareSingleton (SerialManager, true);

    SerialManager();
    virtual ~SerialManager();

    OwnedArray<SerialPortInfo> portInfos;
    OwnedArray<SerialPort> openedPorts;

    void init();
    void updateDeviceList();

    SerialPort* getPort (SerialPortInfo*   portInfo, bool createIfNotThere = true, int openBaudRate = 9600);
    SerialPort* getPort (String hardwareID, String portName, bool createIfNotThere);

    void removePort (SerialPort* p);

    class SerialManagerListener
    {
    public:
        virtual ~SerialManagerListener() {}
        virtual void portAdded (SerialPortInfo*  info) = 0;
        virtual void portRemoved (SerialPortInfo*  info) = 0;
    };

    ListenerList<SerialManagerListener> listeners;
    void addSerialManagerListener (SerialManagerListener* newListener) { listeners.add (newListener); }
    void removeSerialManagerListener (SerialManagerListener* listener) { listeners.remove (listener); }


    // Inherited via Timer
    virtual void timerCallback() override;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (SerialManager)
};


#endif  // SERIALMANAGER_H_INCLUDED
