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


#ifndef SERIALCONTROLLER_H_INCLUDED
#define SERIALCONTROLLER_H_INCLUDED

#include "SerialManager.h"
#include "../Controller.h"
#include "../../Scripting/Js/JsEnvironment.h"

class ControllerUI;



class SerialController : public Controller, public JsEnvironment,
    public SerialPort::SerialPortListener,
    public SerialManager::SerialManagerListener
{
public:
    DECLARE_OBJ_TYPE_DEFAULTNAME (SerialController, "Serial","access your serial ports");
    virtual ~SerialController();

    String lastOpenedPortID; //for ghosting


    EnumParameter* selectedPort;

//    StringParameter* selectedHardwareID;
    SerialPort* port;
    EnumParameter * protocol;

    
    
    //Script

    BoolParameter* logIncoming;

    void onContainerParameterChanged ( ParameterBase* p) override;
    



    void buildLocalEnv() override;
    static var sendMessageFromScript (const var::NativeFunctionArgs& v);

    //Device info
    String deviceID;


    //LGML Serial functions

    void sendIdentificationQuery();
    virtual void processMessage (const String& message);

    // Inherited via SerialPortListener

    virtual void portOpened (SerialPort*) override;
    virtual void portClosed (SerialPort*) override;
    virtual void portRemoved (SerialPort*) override;
    virtual void serialDataReceived (const var& data) override;



    // Inherited via SerialManagerListener
    virtual void portAdded (SerialPortInfo* info) override;
    virtual void portRemoved (SerialPortInfo* info) override;

private:
    void setCurrentPort (SerialPort* port);
    void checkAndAddParameterIfNeeded (const StringArray& msg);
    void applyProtocol();
};



#endif  // SERIALCONTROLLER_H_INCLUDED
