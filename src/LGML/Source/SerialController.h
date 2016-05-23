/*
  ==============================================================================

    SerialController.h
    Created: 22 May 2016 4:50:41pm
    Author:  bkupe

  ==============================================================================
*/

#ifndef SERIALCONTROLLER_H_INCLUDED
#define SERIALCONTROLLER_H_INCLUDED

#include "SerialManager.h"
#include "Controller.h"
#include "JsEnvironment.h"

class ControllerUI;


class SerialController : public Controller, public JsEnvironment,
	public SerialPort::SerialPortListener,
	public SerialManager::SerialManagerListener
{
public:
	SerialController();
	virtual ~SerialController();

	String lastOpenedPortID; //for ghosting

	SerialPort * port;
	void setCurrentPort(SerialPort *port);

	void buildLocalEnv() override;
	static var sendMessageFromScript(const var::NativeFunctionArgs & v);

	// Inherited via SerialPortListener

	virtual void portOpened(SerialPort *) override;
	virtual void portClosed(SerialPort *) override;
	virtual void portRemoved(SerialPort *) override;

	ControllerUI * createUI() override;


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
