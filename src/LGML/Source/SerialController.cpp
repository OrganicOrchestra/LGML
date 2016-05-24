/*
  ==============================================================================

    SerialController.cpp
    Created: 22 May 2016 4:50:41pm
    Author:  bkupe

  ==============================================================================
*/

#include "SerialController.h"
#include "SerialControllerUI.h"

SerialController::SerialController() :
	JsEnvironment("Serial.serial"),
	port(nullptr)
{
	SerialManager::getInstance()->addSerialManagerListener(this);
}

SerialController::~SerialController()
{
	if (SerialManager::getInstanceWithoutCreating() != nullptr)
	{
		SerialManager::getInstance()->removeSerialManagerListener(this);
	}

	setCurrentPort(nullptr);
}

void SerialController::setCurrentPort(SerialPort * _port)
{
	if (port == _port) return;
	if (port != nullptr)
	{
		port->removeSerialPortListener(this);
	}
	port = _port;

	if (port != nullptr)
	{
		port->addSerialPortListener(this);
		lastOpenedPortID = port->info->hardwareID;
	}

	serialControllerListeners.call(&SerialControllerListener::currentPortChanged);
}

void SerialController::buildLocalEnv() {
	DynamicObject obj;
	static const Identifier jsSendMessageIdentifier("sendMessage");
	obj.setMethod(jsSendMessageIdentifier, sendMessageFromScript);
	obj.setProperty(jsPtrIdentifier, (int64)this);

	setLocalNamespace(obj);
}

void SerialController::portOpened(SerialPort *)
{
	serialControllerListeners.call(&SerialControllerListener::portOpened);

	//tmp
	port->writeString("i");
}

void SerialController::portClosed(SerialPort *)
{
	serialControllerListeners.call(&SerialControllerListener::portClosed);
}

void SerialController::portRemoved(SerialPort *)
{
	setCurrentPort(nullptr);
}

void SerialController::serialDataReceived(const var & data)
{
	DBG("Data received in controller : " << data.toString());
}

var SerialController::sendMessageFromScript(const var::NativeFunctionArgs &) {
	//    SerialController * c = getObjectPtrFromJS<SerialController>(a);
	return var::undefined();
}

ControllerUI * SerialController::createUI()
{
	return new SerialControllerUI(this);
}

void SerialController::portAdded(SerialPortInfo * info)
{
	DBG("SerialController, portAdded >" << info->hardwareID << "< > " << lastOpenedPortID);
	if (port == nullptr && lastOpenedPortID == info->hardwareID)
	{
		setCurrentPort(SerialManager::getInstance()->getPort(info));
	}
}

void SerialController::portRemoved(SerialPortInfo *)
{
}
