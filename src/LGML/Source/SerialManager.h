/*
  ==============================================================================

    SerialManager.h
    Created: 22 May 2016 5:19:58pm
    Author:  bkupe

  ==============================================================================
*/

#ifndef SERIALMANAGER_H_INCLUDED
#define SERIALMANAGER_H_INCLUDED

#include "SerialPort.h"



class SerialManager :
	public Timer
{
public:
	juce_DeclareSingleton(SerialManager, true);

	SerialManager();
	virtual ~SerialManager();

	OwnedArray<SerialPortInfo> portInfos;
	OwnedArray<SerialPort> openedPorts;

	void init();
	void updateDeviceList();

	SerialPort * getPort(SerialPortInfo *  portInfo, bool createIfNotThere = true, int openBaudRate = 9600);
  SerialPort * getPort(String hardwareID, String portName, bool createIfNotThere);
  
	void removePort(SerialPort * p);

	class SerialManagerListener
	{
	public:
		virtual ~SerialManagerListener() {}
		virtual void portAdded(SerialPortInfo  *info) = 0;
		virtual void portRemoved(SerialPortInfo  *info) = 0;
	};

	ListenerList<SerialManagerListener> listeners;
	void addSerialManagerListener(SerialManagerListener* newListener) { listeners.add(newListener); }
	void removeSerialManagerListener(SerialManagerListener* listener) { listeners.remove(listener); }


	// Inherited via Timer
	virtual void timerCallback() override;

	JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(SerialManager)
};


#endif  // SERIALMANAGER_H_INCLUDED
