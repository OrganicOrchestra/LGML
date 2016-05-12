/*
  ==============================================================================

    OSCController.cpp
    Created: 2 Mar 2016 8:50:08pm
    Author:  bkupe

  ==============================================================================
*/

#include "OSCController.h"
#include "OSCControllerUI.h"

OSCController::OSCController(const String &_name) :
    Controller(_name)
{

    localPortParam = addStringParameter("Local Port", "The port to bind for the controller to receive OSC from it","11000");

    remoteHostParam = addStringParameter("Remote Host", "The host's IP of the remote controller","127.0.0.1");
    remotePortParam = addStringParameter("Remote Port", "The port bound by the controller to send OSC to it","8000");

    setupReceiver();
    setupSender();

    receiver.addListener(this);

	
}

OSCController::~OSCController()
{

}

void OSCController::setupReceiver()
{
    DBG("setupReceiver");
    receiver.disconnect();
    receiver.connect(localPortParam->stringValue().getIntValue());
    //DBG("Receiver connected" + String(result));
}
void OSCController::setupSender()
{
    DBG("Resetup sender with " << remoteHostParam->stringValue() << ":" << remotePortParam->stringValue().getIntValue());
    sender.disconnect();
    sender.connect(remoteHostParam->stringValue(), remotePortParam->stringValue().getIntValue());
}

void OSCController::processMessage(const OSCMessage & msg)
{
	if (!enabledParam->boolValue()) return;

	bool result = processMessageInternal(msg);
	oscListeners.call(&OSCControllerListener::messageProcessed, msg, result);
}

Result OSCController::processMessageInternal(const OSCMessage &)
{
	return Result::fail("Not handled"); //if not overriden, msg is not handled so result is false
}

void OSCController::onContainerParameterChanged(Parameter * p)
{
	Controller::onContainerParameterChanged(p);

    if (p == localPortParam) setupReceiver();
    else if (p == remotePortParam || p == remoteHostParam) setupSender();
}

void OSCController::oscMessageReceived(const OSCMessage & message)
{
    //DBG("Message received !");
    processMessage(message);
}

void OSCController::oscBundleReceived(const OSCBundle & bundle) 
{
	for (auto &m : bundle)
	{
		processMessage(m.getMessage());
	}
}

ControllerUI * OSCController::createUI()
{
	return new OSCControllerUI(this);
}
