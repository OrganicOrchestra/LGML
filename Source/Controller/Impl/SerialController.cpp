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


#include "SerialController.h"

#include "../../Scripting/Js/JsHelpers.h"


#include "../ControllerFactory.h"
REGISTER_OBJ_TYPE_NAMED (Controller, SerialController, "t_Serial");




SerialController::SerialController (StringRef name) :
    JsEnvironment ("controllers.serial", this),
    Controller (name),
    port (nullptr)
{

    setNamespaceName ("controllers." + shortName);
    logIncoming = addNewParameter<BoolParameter> ("logIncoming", "log Incoming midi message", false);

//    selectedHardwareID = addNewParameter<StringParameter> ("selectedHardwareID", "Id of the selected hardware", "");
    selectedPort = addNewParameter<EnumParameter> ("selectedPort", "Name of the selected hardware",SerialManager::getInstance(), "");

    SerialManager::getInstance()->addSerialManagerListener (this);
}

SerialController::~SerialController()
{
    if (SerialManager::getInstanceWithoutCreating() != nullptr)
    {
        SerialManager::getInstance()->removeSerialManagerListener (this);
    }

    setCurrentPort (nullptr);
}

void SerialController::setCurrentPort (SerialPort* _port)
{

    if (port == _port) return;


    if (port != nullptr)
    {

        port->removeSerialPortListener (this);
    }

    port = _port;

    if (port != nullptr)
    {
        port->addSerialPortListener (this);
        lastOpenedPortID = port->info->port;

//        selectedPort->setValue (port->info->port);
//        selectedHardwareID->setValue (port->info->hardwareID);

        sendIdentificationQuery();
    }
    else
    {
        DBG ("set port null");
    }

    DBG ("current port changed");
    serialControllerListeners.call (&SerialControllerListener::currentPortChanged);
}

void SerialController::newJsFileLoaded()
{

}

void SerialController::onContainerParameterChanged (Parameter* p)
{
    Controller::onContainerParameterChanged (p);

    if (p == nameParam)
    {
        setNamespaceName ("controllers." + shortName);
    }
    else if ( p == selectedPort)
    {
        String portName = selectedPort->getFirstSelectedId().toString();
        String portHID = selectedPort->getFirstSelectedValue().toString();
        SerialPort* _port  = SerialManager::getInstance()->getPort (portHID, portName, true);

        if (_port != nullptr)
        {
            setCurrentPort (_port);
        }
    }


};

void SerialController::buildLocalEnv()
{

    DynamicObject obj;
    static const Identifier jsSendMessageIdentifier ("sendMessage");
    obj.setMethod (jsSendMessageIdentifier, sendMessageFromScript);
//    obj.setProperty (jsPtrIdentifier, (int64)this);



    setLocalNamespace (obj);
}

void SerialController::portOpened (SerialPort* )
{
    serialControllerListeners.call (&SerialControllerListener::portOpened);

    sendIdentificationQuery();
}

void SerialController::portClosed (SerialPort*)
{
    serialControllerListeners.call (&SerialControllerListener::portClosed);
}

void SerialController::portRemoved (SerialPort*)
{
    setCurrentPort (nullptr);
}

void SerialController::serialDataReceived (const var& data)
{

    processMessage (data.toString());
    inActivityTrigger->trigger();
}

void SerialController::controllableAdded (ControllableContainer*, Controllable* c)
{
    if (c->isUserDefined)
    {
        reloadFile();
    }
}
void SerialController::controllableRemoved (ControllableContainer*, Controllable* c)
{
    if (c->isUserDefined)
    {
        reloadFile();
    }

}
var SerialController::sendMessageFromScript (const var::NativeFunctionArgs&)
{
    //    SerialController * c = getObjectPtrFromJS<SerialController>(a);
    return var::undefined();
}

void SerialController::sendIdentificationQuery()
{
    outActivityTrigger->trigger();
    port->writeString ("i");
}

void SerialController::processMessage (const String& message)
{
    StringArray split;
    split.addTokens (message.removeCharacters ("\n"), true);
    String command = split[0];

    if (command == "i")
    {
        //identification
        deviceID = split[1];

        while (serialVariables.size() > 0)
        {
            serialVariables.removeAllInstancesOf (serialVariables[0]);
        }

    }
    else if (command == "a")
    {
        auto found = userContainer.getControllableForAddress (split[1]);

        if (!found )
        {
            FloatParameter* v ;

            if (split.size() >= 4)
            {
                v = userContainer.addNewParameter<FloatParameter> (split[1], split[1],
                                                                   split[2].getFloatValue(),
                                                                   split[2].getFloatValue(),
                                                                   split[3].getFloatValue());
            }
            else
            {
                v = userContainer.addNewParameter<FloatParameter> (split[1], split[1], 0.f);
            }

            serialVariables.add (v);
        }
    }
    else if (command == "d")
    {
        auto found = userContainer.getControllableForAddress (split[1]);

        if (!found )
        {
            BoolParameter* v = userContainer.addNewParameter<BoolParameter> (split[1], split[1], false);
            serialVariables.add (v);
        }
    }
    else if (command == "u")
    {
        auto v = userContainer.getControllableForAddress (split[1]);

        if (v != nullptr)
        {
            ((Parameter*)v)->setValue (split[2].getFloatValue());
        }
    }
}



void SerialController::portAdded (SerialPortInfo* info)
{
    //DBG("SerialController, portAdded >" << info->hardwareID << "< > " << lastOpenedPortID);
    if (port == nullptr && lastOpenedPortID == info->port)
    {
        setCurrentPort (SerialManager::getInstance()->getPort (info));
    }
}

void SerialController::portRemoved (SerialPortInfo*)
{
}
