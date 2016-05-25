/*
  ==============================================================================

    SerialPort.cpp
    Created: 25 May 2016 4:47:22pm
    Author:  bkupe

  ==============================================================================
*/

#include "SerialPort.h"
#include "SerialManager.h"

SerialPort::SerialPort(Serial * _port, SerialPortInfo  * _info, PortMode _mode) :
	info(_info),
	port(_port),
	mode(_mode),
	thread(_info->port, this)
{
	open();
}

SerialPort::~SerialPort()
{
	//cleanup
	close();
	listeners.call(&SerialPortListener::portRemoved, this);
}

void SerialPort::open()
{
	if (!port->isOpen())
	{
		port->open();
	}
	
	port->setDTR();
	port->setRTS();

	if (!thread.isThreadRunning())
	{
		thread.startThread();
		thread.addAsyncSerialListener(this);
		listeners.call(&SerialPortListener::portOpened, this);
	}
}

void SerialPort::close()
{
	if (port->isOpen())
	{
		thread.removeAsyncSerialListener(this);
		thread.stopThread(10000);
		port->close();
		listeners.call(&SerialPortListener::portClosed, this);
	}
}

bool SerialPort::isOpen() {
	if (port == nullptr) return false;
	return port->isOpen();
}

int SerialPort::writeString(String message, bool endLine)
{
	
	if (!port->isOpen()) return 0;

	DBG("Write string : " << message << " -- endline ? " << String(endLine));
	String m = message;
	if (endLine) m += "\n";
	return (int)port->write(m.toStdString());
}

int SerialPort::writeBytes(Array<uint8_t> data)
{
	return (int)port->write(data.getRawDataPointer(), data.size());
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
	port(_port)
{
}

SerialReadThread::~SerialReadThread()
{
	stopThread(100);
}

void SerialReadThread::run()
{
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

	
}