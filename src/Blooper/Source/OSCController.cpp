/*
  ==============================================================================

    OSCController.cpp
    Created: 2 Mar 2016 8:50:08pm
    Author:  bkupe

  ==============================================================================
*/

#include "OSCController.h"

OSCController::OSCController(const String &name) :
    Controller(name)
{

    DBG("OSC COntroller init");
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
    receiver.connect(localPortParam->value.getIntValue());
	//DBG("Receiver connected" + String(result));
}
void OSCController::setupSender()
{
    DBG("Resetup sender with " + remoteHostParam->value + ":" + remotePortParam->value);
    sender.disconnect();
    sender.connect(remoteHostParam->value, remotePortParam->value.getIntValue());
}

void OSCController::processMessage(const OSCMessage &)
{
    //to override
}

void OSCController::parameterValueChanged(Parameter * p)
{
    Controller::parameterValueChanged(p);
    if (p == localPortParam) setupReceiver();
    else if (p == remotePortParam || p == remoteHostParam) setupSender();
}

void OSCController::oscMessageReceived(const OSCMessage & message)
{
    DBG("Message received !");
    processMessage(message);
}
