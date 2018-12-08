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
REGISTER_OBJ_TYPE_NAMED (Controller, SerialController, "t_Serial","Serial");

/*
static Identifier LGMLProtocol("LGML");
static Identifier RAWProtocol("RAW");
class SerialProtocol : public EnumParameterModel{
    public:
    SerialProtocol(){
        
        addOption(LGMLProtocol,LGMLProtocol.toString(),true);
        addOption(RAWProtocol,RAWProtocol.toString(),true);
    };



};

static SerialProtocol serialProtocolModel;
*/
//#include "../../Controllable/Parameter/FileParameter.h" // TODO check why userDefined parameter needs to trigger changes

SerialController::SerialController (StringRef name) :
JsEnvironment ("controllers.serial", this),
Controller (name),
port (nullptr)
{


    setNamespaceName ("controllers." + shortName);
    logIncoming = addNewParameter<BoolParameter> ("logIncoming", "log Incoming midi message", false);
    selectedPort = addNewParameter<EnumParameter> ("selectedPort", "Name of the selected hardware",SerialManager::getInstance(), "");
    //  protocol =addNewParameter<EnumParameter> ("protocol", "which serial protocol to use",&serialProtocolModel,LGMLProtocol.toString());

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

    if (port != nullptr)
    {
        port->removeSerialPortListener (this);
    }

    port = _port;

    if (port != nullptr)
    {
        port->addSerialPortListener (this);
        lastOpenedPortID = port->info->port;
        sendIdentificationQuery();
        applyProtocol();
    }
    else
    {
        selectedPort->unselectAll();
        DBG ("set port null");
    }
    isConnected->setValue(port!=nullptr);
    DBG ("current port changed");

}


void SerialController::onContainerParameterChanged ( ParameterBase* p)
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


        setCurrentPort (_port);

    }
    else if(p==protocol){
        applyProtocol();
    }


};



void SerialController::applyProtocol(){
    /*
    if(port){
        if(protocol->getFirstSelectedId()==LGMLProtocol){
            port->mode=SerialPort::PortMode::LINES;
        }

        else if(protocol->getFirstSelectedId()==RAWProtocol){
            port->mode=SerialPort::PortMode::RAW;
        }
    }
 */
}

void SerialController::buildLocalEnv()
{

    DynamicObject obj;
    static const Identifier jsSendMessageIdentifier ("sendMessage");
    obj.setMethod (jsSendMessageIdentifier, sendMessageFromScript);



    setLocalNamespace (obj);
}

void SerialController::portOpened (SerialPort* )
{
    sendIdentificationQuery();
    isConnected->setValue(true);
}

void SerialController::portClosed (SerialPort*)
{
    isConnected->setValue(false);
}

void SerialController::portRemoved (SerialPort*)
{
    setCurrentPort (nullptr);
}

void SerialController::serialDataReceived (const var& data)
{

    processMessage (data.toString());
    if(logIncoming->boolValue()){NLOG (getNiceName(),"In : "+ data.toString());}
    inActivityTrigger->triggerDebounced(activityTriggerDebounceTime);

}


var SerialController::sendMessageFromScript (const var::NativeFunctionArgs&)
{
    //    SerialController * c = getObjectPtrFromJS<SerialController>(a);
    return var::undefined();
}

void SerialController::sendIdentificationQuery()
{
    outActivityTrigger->triggerDebounced(activityTriggerDebounceTime);
    port->writeString ("i");
}

void SerialController::processMessage (const String& message)
{
    // if(protocol->getFirstSelectedId()==LGMLProtocol){
        StringArray split;
        split.addTokens (message.removeCharacters ("\n"), true);
        String command = split[0];

        if (command == "i")
        {
            //identification
            deviceID = split[1];

        }
        else if (command == "a")
        {
            auto found = userContainer.getControllableForAddress (split[1]);

            if (!found )
            {
                //            FloatParameter* v ;

                if (split.size() >= 4)
                {
                    userContainer.addNewParameter<FloatParameter> (split[1], split[1],
                                                                   split[2].getFloatValue(),
                                                                   split[2].getFloatValue(),
                                                                   split[3].getFloatValue());
                }
                else
                {
                    userContainer.addNewParameter<FloatParameter> (split[1], split[1], 0.f);
                }


            }
        }
        else if (command == "d")
        {
            auto found = userContainer.getControllableForAddress (split[1]);

            if (!found )
            {
                userContainer.addNewParameter<BoolParameter> (split[1], split[1], false);

            }
        }
        else if (command == "u")
        {
            auto v = userContainer.getControllableForAddress (split[1]);

            if (v != nullptr)
            {
                (( ParameterBase*)v)->setValue (split[2].getFloatValue());
            }
        }
/*    }// use_LGMLProtocol
    else{
        StringArray split;
        split.addTokens (message.removeCharacters ("\n"), true);
        String command = split[0];
        auto v = userContainer.getControllableForAddress (command);
        if (v != nullptr){
            (( ParameterBase*)v)->setValue (split[1].getFloatValue());
        }
        else if( autoAddParams){
            MessageManager::getInstance()->callAsync ([this, split]() {checkAndAddParameterIfNeeded (split);});
        }
    }
*/
}

StringArray OSCAddressToArray (const String& addr)
{
    StringArray addrArray;
    addrArray.addTokens (addr, juce::StringRef ("/"), juce::StringRef ("\""));
    addrArray.remove (0);
    return addrArray;
}


void SerialController::checkAndAddParameterIfNeeded (const StringArray& split)
{
    if(split.size()==0)return;

    String command = split[0];


    auto* linked = ParameterBase::fromControllable (userContainer.getControllableForAddress (command));

    if (!linked)
    {

        StringArray sa = OSCAddressToArray (command);
        ParameterContainer* tC = &userContainer;

        for ( int i = 0 ; i < sa.size() - 1 ; i++)
        {
            auto* c = dynamic_cast<ParameterContainer*> (tC->getControllableContainerByName (sa[i], true));

            if (!c)
            {
                c = new ParameterContainer (sa[i]);
                c->setUserDefined (true);
                tC->addChildControllableContainer (c, true);
            }

            tC = c ? c : nullptr;
        }

        String pName = sa[sa.size() - 1];

        if (tC)
        {
            if (split.size() == 1)
            {
                linked = tC->addNewParameter<Trigger> (pName, "entry for " + command);
            }
            else
            {
                if(split[1].containsOnly("123456789.")){
                    linked = tC->addNewParameter<FloatParameter> (pName, "entry for " + command);
                }
                else if (split[1].containsOnly("10"))
                {
                    linked = tC->addNewParameter<BoolParameter> (pName, "entry for " + command);
                }
            }

            if (linked)
            {
                (( ParameterBase*)linked)->setValue (split[2].getFloatValue());
            }
        }
        else
        {
            jassertfalse;
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
