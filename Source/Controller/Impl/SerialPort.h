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


#ifndef SERIALPORT_H_INCLUDED
#define SERIALPORT_H_INCLUDED


#define SYNCHRONOUS_SERIAL_LISTENERS

#include "../../Utils/QueuedNotifier.h"

#if !defined __arm__
    #define SERIALSUPPORT 1
#else
    #define SERIALSUPPORT 0
#endif

#if SERIALSUPPORT
    #include "serial/serial.h"
    using namespace serial;
#endif


class SerialPort;

class SerialReadThread :
    public Thread
{
public:

    SerialReadThread (String name, SerialPort* _port);
    virtual ~SerialReadThread();

    SerialPort* port;

    virtual void run() override;

    // ASYNC
    std::unique_ptr<QueuedNotifier<var>> queuedNotifier;
    typedef QueuedNotifier<var>::Listener AsyncListener;

    class SerialThreadListener
    {
    public:
        virtual ~SerialThreadListener() {}
        virtual void newMessage (const var&) {}
    };

    ListenerList<SerialThreadListener> serialThreadListeners;

    void addSerialListener (SerialThreadListener* newListener) { serialThreadListeners.add (newListener); }
    void removeSerialListener (SerialThreadListener* listener) { serialThreadListeners.remove (listener); }
    void addAsyncSerialListener (AsyncListener* newListener) ;
    void removeAsyncSerialListener (AsyncListener* listener) ;

};

class SerialPortInfo
{
public:
    SerialPortInfo (String _port, String _description, String _hardwareID) :
        port (_port), description (_description), hardwareID (_hardwareID)
    {}

    virtual ~SerialPortInfo() {}

    String port;
    String description;
    String hardwareID;


};

class SerialPort :
#ifdef SYNCHRONOUS_SERIAL_LISTENERS
    public SerialReadThread::SerialThreadListener
#else
    public SerialReadThread::AsyncListener
#endif
{
public:
    SerialReadThread thread;

    enum PortMode { LINES, DATA255, RAW, COBS };

#if SERIALSUPPORT
    SerialPort (Serial* port, SerialPortInfo* info, PortMode mode = LINES);
    std::unique_ptr<Serial> port;
#else
    SerialPort (SerialPortInfo* info, PortMode mode = LINES);
#endif

    virtual ~SerialPort();

    SerialPortInfo* info;

    PortMode mode;

    void open();
    void close(bool stopThread = true);

    bool isOpen();

    //write functions
    int writeString (String message, bool endLine = true);
    int writeBytes (Array<uint8_t> data);

    virtual void newMessage (const var& data) override;

    class SerialPortListener
    {
    public:
        virtual ~SerialPortListener() {}

        //serial data here
        virtual void portOpened (SerialPort*) {}
        virtual void portClosed (SerialPort*) {}
        virtual void portRemoved (SerialPort*) {}
        virtual void serialDataReceived (const var&) {}
    };

    ListenerList<SerialPortListener> listeners;
    void addSerialPortListener (SerialPortListener* newListener);
    void removeSerialPortListener (SerialPortListener* listener);
};




#endif  // SERIALPORT_H_INCLUDED
