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
	Controller("Serial"),
	port(nullptr)
{

	setNamespaceName("Serial." + nameParam->stringValue());
	scriptPath = addStringParameter("jsScriptPath", "path for js script", "");
	logIncoming = addBoolParameter("logIncoming", "log Incoming midi message", false);


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
		sendIdentificationQuery();
	}

	serialControllerListeners.call(&SerialControllerListener::currentPortChanged);
}

void SerialController::newJsFileLoaded()
{
	scriptPath->setValue(currentFile.getFullPathName());
}

void SerialController::buildLocalEnv() {

	DynamicObject obj;
	static const Identifier jsSendMessageIdentifier("sendMessage");
	obj.setMethod(jsSendMessageIdentifier, sendMessageFromScript);
	obj.setProperty(jsPtrIdentifier, (int64)this);

	for (auto &v : variables)
	{
		obj.setProperty(v->parameter->shortName, v->parameter->createDynamicObject());
	}

	setLocalNamespace(obj);
}

void SerialController::portOpened(SerialPort *)
{
	serialControllerListeners.call(&SerialControllerListener::portOpened);

	sendIdentificationQuery();
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
	activityTrigger->trigger();
	processMessage(data.toString());
}

var SerialController::sendMessageFromScript(const var::NativeFunctionArgs &) {
	//    SerialController * c = getObjectPtrFromJS<SerialController>(a);
	return var::undefined();
}

void SerialController::sendIdentificationQuery()
{
	port->writeString("i");
}

void SerialController::processMessage(const String & message)
{
	StringArray split;
	split.addTokens(message.removeCharacters("\n"),true);
	String command = split[0];
	if (command == "i")
	{
		//identification
		deviceID = split[1];
		while (serialVariables.size() > 0)
		{
			removeVariable(serialVariables[0]);
			serialVariables.removeAllInstancesOf(serialVariables[0]);
		}

	} else if (command == "a")
	{
		if (getVariableForName(split[1]) == nullptr)
		{
			ControlVariable * v = addVariable(new FloatParameter(split[1], split[1], 0));
			serialVariables.add(v);
		}
	} else if (command == "d")
	{
		if (getVariableForName(split[1]) == nullptr)
		{
			ControlVariable * v = addVariable(new BoolParameter(split[1], split[1], false));
			serialVariables.add(v);
		}
	} else if (command == "u")
	{
		ControlVariable *  v = getVariableForName(split[1]);
		if (v != nullptr)
		{
			v->parameter->setValue(split[2].getFloatValue());
		}
	}
}

ControllerUI * SerialController::createUI()
{
	return new SerialControllerUI(this);
}

void SerialController::portAdded(SerialPortInfo * info)
{
	//DBG("SerialController, portAdded >" << info->hardwareID << "< > " << lastOpenedPortID);
	if (port == nullptr && lastOpenedPortID == info->hardwareID)
	{
		setCurrentPort(SerialManager::getInstance()->getPort(info));
	}
}

void SerialController::portRemoved(SerialPortInfo *)
{
}
