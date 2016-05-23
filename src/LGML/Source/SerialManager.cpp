/*
  ==============================================================================

    SerialManager.cpp
    Created: 22 May 2016 5:19:58pm
    Author:  bkupe

  ==============================================================================
*/

#include "SerialManager.h"

juce_ImplementSingleton(SerialManager)

SerialManager::SerialManager()
{
	
}

SerialManager::~SerialManager()
{
	stopTimer();
}

void SerialManager::init()
{
	startTimer(1000);
}

void SerialManager::updateDeviceList()
{

	std::vector<serial::PortInfo> devices_found = serial::list_ports();
	std::vector<serial::PortInfo>::iterator iter = devices_found.begin();

	OwnedArray<SerialPortInfo> newInfos;
	
	Array<SerialPortInfo *> portsToNotifyAdded;
	Array<SerialPortInfo *> portsToNotifyRemoved;

	while (iter != devices_found.end())
	{
		serial::PortInfo device = *iter++;
		newInfos.add(new SerialPortInfo(device.port,device.description,device.hardware_id));
	}

	//check added devices
	for (auto &newD : newInfos)
	{
		bool found = false;
		for (auto &sourceD : portInfos)
		{
			if (sourceD->hardwareID == newD->hardwareID)
			{
				found = true;
				break;
			}
		}

		if (!found)
		{
			portsToNotifyAdded.add(newD);
		}
	}

	//check removed
	for (auto &sourceD : portInfos)
	{
		bool found = false;
		for (auto &newD : newInfos)
		{
			if (sourceD->hardwareID == newD->hardwareID)
			{
				found = true;
				break;
			}
		}

		if (!found)
		{
			//create one so portInfos can delete the original
			portsToNotifyRemoved.add(sourceD);
		}
	}

	//check removed devices
	
	
	for (auto &p : portsToNotifyRemoved)
	{
		DBG("Port Removed " << p->description);
		

		portInfos.removeObject(p,false);
		listeners.call(&SerialManagerListener::portRemoved, p);

		SerialPort * port = getPort(p, false);
		if (port != nullptr) removePort(port);
		delete p;
	}

	for (auto &p : portsToNotifyAdded)
	{
		DBG("Port added " << p->description);
		
		newInfos.removeObject(p, false);
		portInfos.add(p);
		listeners.call(&SerialManagerListener::portAdded, p);

	}
}

SerialPort * SerialManager::getPort(SerialPortInfo * portInfo, bool createIfNotThere, int openBaudRate)
{
	for (auto & sp : openedPorts)
	{
		if (sp->info->hardwareID == portInfo->hardwareID) return sp;
	}

	if (createIfNotThere)
	{
		Serial * newSerial = new Serial(portInfo->port.toStdString(), openBaudRate, serial::Timeout::simpleTimeout(1000));
		SerialPort *p =new SerialPort(newSerial, portInfo);
		openedPorts.add(p);
		return p;
	}

	return nullptr;
}

void SerialManager::removePort(SerialPort * p)
{
	openedPorts.removeObject(p, true);
}

void SerialManager::timerCallback()
{
	updateDeviceList();
}

SerialPort::SerialPort(Serial * _port, SerialPortInfo  * _info, PortMode _mode) :
	info(_info),
	port(_port),
	mode(_mode),
	thread(_info->port,this)
{
	thread.addAsyncSerialListener(this);
}

SerialPort::~SerialPort()
{
	//cleanup
	thread.removeAsyncSerialListener(this);
	close();
	listeners.call(&SerialPortListener::portRemoved, this);
}

void SerialPort::open()
{
	if (!port->isOpen())
	{
		port->open();
		listeners.call(&SerialPortListener::portOpened, this);
	}
}

void SerialPort::close()
{
	if (port->isOpen())
	{
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
	String m = message;
	if (endLine) m = +"\n";

	return (int)port->write(m.toStdString());
}

int SerialPort::writeBytes(Array<uint8_t> data)
{
	return (int)port->write(data.getRawDataPointer(),data.size());
}

void SerialPort::newMessage(const var & data) {
	DBG("Got data in serialPort, dispatch");
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
	shouldExit(false)
{
	startThread();
}

SerialReadThread::~SerialReadThread()
{
	stopThread(500);
}

void SerialReadThread::run()
{
	Array<int> byteBuffer;

	while (!shouldExit)
	{
		if (!port->isOpen()) continue;
		size_t numBytes = port->port->available();
		if (numBytes == 0) continue;

		try
		{
			switch (port->mode)
			{
			case SerialPort::PortMode::LINES:
			{
				std::string line = port->port->readline();
				DBG("Check for line : " << line);
				if (line.size() > 0)
				{
					DBG("Got line !");
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
		} catch (SerialException e)
		{

		}

	}

	sleep(10); //100fps
}
