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


#include "SerialPort.h"
#include "SerialManager.h"

#if SERIALSUPPORT
	SerialPort::SerialPort(Serial * _port, SerialPortInfo  * _info, PortMode _mode) :
		info(_info),
		port(_port),
		mode(_mode),
		thread(_info->port, this)
	{
		open();
	}
#else
	SerialPort::SerialPort(SerialPortInfo  * _info, PortMode _mode) :
		info(_info),
		mode(_mode),
		thread(_info->port, this)
	{
		open();
	}
#endif

SerialPort::~SerialPort()
{
	//cleanup
	close();
	listeners.call(&SerialPortListener::portRemoved, this);
}

void SerialPort::open()
{
#if SERIALSUPPORT
	if (!port->isOpen())
	{
		port->open();
	}

	port->setDTR();
	port->setRTS();

	if (!thread.isThreadRunning())
	{
		thread.startThread();
#ifdef SYNCHRONOUS_SERIAL_LISTENERS
    thread.addSerialListener(this);
#else
		thread.addAsyncSerialListener(this);
#endif
		listeners.call(&SerialPortListener::portOpened, this);
	}
#endif
}

void SerialPort::close()
{
#if SERIALSUPPORT
	if (port->isOpen())
	{
#ifdef SYNCHRONOUS_SERIAL_LISTENERS
    thread.removeSerialListener(this);
#else
    thread.removeAsyncSerialListener(this);
#endif

		thread.stopThread(10000);
		port->close();
		listeners.call(&SerialPortListener::portClosed, this);
	}
#endif
}

bool SerialPort::isOpen() {
#if SERIALSUPPORT
	if (port == nullptr) return false;
	return port->isOpen();
#else
	return false;
#endif
}

int SerialPort::writeString(String message, bool endLine)
{
#if SERIALSUPPORT
	if (!port->isOpen()) return 0;

	DBG("Write string : " << message << " -- endline ? " << (endLine?"1":"0"));
	String m = message;
	if (endLine) m += "\n";
	return (int)port->write(m.toStdString());
#else
	return 0;
#endif
}

int SerialPort::writeBytes(Array<uint8_t> data)
{
#if SERIALSUPPORT
	return (int)port->write(data.getRawDataPointer(), data.size());
#else
	return 0;
#endif
}

void SerialPort::newMessage(const var & data) {
	listeners.call(&SerialPortListener::serialDataReceived, data);
}

void SerialPort::addSerialPortListener(SerialPortListener * newListener) { listeners.add(newListener); }

void SerialPort::removeSerialPortListener(SerialPortListener * listener) {
	listeners.remove(listener);
	if (listeners.size() == 0) SerialManager::getInstance()->removePort(this);
}

SerialReadThread::SerialReadThread(String name, SerialPort * _port) :
	Thread(name + "_thread"),
	port(_port),
    queuedNotifier(100)
{
}

SerialReadThread::~SerialReadThread()
{
	stopThread(100);
}

void SerialReadThread::run()
{
#if SERIALSUPPORT
	Array<int> byteBuffer;
	while (!threadShouldExit())
	{
		sleep(10); //100fps

		if (port == nullptr) return;
		if (!port->isOpen()) return;

		try
		{

			int numBytes = (int)port->port->available();
			if (numBytes == 0) continue;
			switch (port->mode)
			{

			case SerialPort::PortMode::LINES:
			{

				std::string line = port->port->readline();
				if (line.size() > 0)
				{
          serialThreadListeners.call(&SerialThreadListener::newMessage,var(line));
					queuedNotifier.addMessage(new var(line));
				}

			}
			break;

			case SerialPort::PortMode::RAW:
			{
				/*
				var * dataVar = new var();
				Array<int> bytes;
				while (port->port->available())
				{
				std::vector<uint8_t> data;
				port->port->read(data);
				bytes.addArray(data);
				for(auto &b:bytes) dataVar->append(b);
				}

				queuedNotifier.addMessage(dataVar);
				*/
			}
			break;

			case SerialPort::PortMode::DATA255:
			{
				while (port->port->available())
				{
					uint8_t b = port->port->read(1)[0];
					if (b == 255)
					{
						var * dataVar = new var();
						for (auto &by : byteBuffer) dataVar->append(by);
            serialThreadListeners.call(&SerialThreadListener::newMessage,*dataVar);
						queuedNotifier.addMessage(dataVar);
						byteBuffer.clear();
					} else
					{
						byteBuffer.add(b);
					}
				}
			}
			break;

			case SerialPort::PortMode::COBS:
			{
				//todo : handle cobs
			}
			break;
			}
		} catch (...)
		{
			DBG("### Serial Problem ");
		}


	}
#endif

}
