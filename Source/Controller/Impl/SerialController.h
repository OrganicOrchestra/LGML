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
	SerialController();
	virtual ~SerialController();

	String lastOpenedPortID; //for ghosting


	StringParameter * selectedPort;
	StringParameter * selectedHardwareID;
	SerialPort * port;
	void setCurrentPort(SerialPort *port);

	Array<Parameter *> serialVariables;
    void controllableAdded(ControllableContainer * ,Controllable*) override;
    void controllableRemoved(ControllableContainer *,Controllable*) override;
	//Script
	
	BoolParameter * logIncoming;

	void onContainerParameterChanged(Parameter * p) override;
	void newJsFileLoaded() override;
	


	void buildLocalEnv() override;
	static var sendMessageFromScript(const var::NativeFunctionArgs & v);

	//Device info
	String deviceID;


	//LGML Serial functions

	void sendIdentificationQuery();
	virtual void processMessage(const String &message);

	// Inherited via SerialPortListener

	virtual void portOpened(SerialPort *) override;
	virtual void portClosed(SerialPort *) override;
	virtual void portRemoved(SerialPort *) override;
	virtual void serialDataReceived(const var &data) override;


	ControllerUI * createUI() override;
  ControllerEditor * createEditor() override;


	class SerialControllerListener
	{
	public:
		virtual ~SerialControllerListener() {}
		virtual void portOpened() {}
		virtual void portClosed() {}
		virtual void currentPortChanged() {}
	};

	ListenerList<SerialControllerListener> serialControllerListeners;
	void addSerialControllerListener(SerialControllerListener* newListener) { serialControllerListeners.add(newListener); }
	void removeSerialControllerListener(SerialControllerListener* listener) { serialControllerListeners.remove(listener); }


	// Inherited via SerialManagerListener
	virtual void portAdded(SerialPortInfo * info) override;
	virtual void portRemoved(SerialPortInfo * info) override;

};



#endif  // SERIALCONTROLLER_H_INCLUDED
