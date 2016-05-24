/*
  ==============================================================================

    OSCController.cpp
    Created: 2 Mar 2016 8:50:08pm
    Author:  bkupe

  ==============================================================================
*/

#include "OSCController.h"
#include "OSCControllerUI.h"
#include "DebugHelpers.h"

OSCController::OSCController(const String &_name) :
    Controller(_name)
{

    localPortParam = addStringParameter("Local Port", "The port to bind for the controller to receive OSC from it","11000");

    remoteHostParam = addStringParameter("Remote Host", "The host's IP of the remote controller","127.0.0.1");
    remotePortParam = addStringParameter("Remote Port", "The port bound by the controller to send OSC to it","8000");

    logIncomingOSC = addBoolParameter("logIncomingOSC", "log the incoming OSC Messages", false);

    setupReceiver();
    setupSender();

    receiver.addListener(this);


}

OSCController::~OSCController()
{

}

void OSCController::setupReceiver()
{
   // DBG("setupReceiver");
    receiver.disconnect();

    if(!receiver.connect(localPortParam->stringValue().getIntValue())){
        LOG("can't connect to local port : " +localPortParam->stringValue());
    };
    //DBG("Receiver connected" + String(result));
}
void OSCController::setupSender()
{
    //DBG("Resetup sender with " << remoteHostParam->stringValue() << ":" << remotePortParam->stringValue().getIntValue());
    sender.disconnect();
    if(!sender.connect(remoteHostParam->stringValue(), remotePortParam->stringValue().getIntValue())){
        LOG("can't connect to send port : " +remoteHostParam->stringValue()+":"+ remotePortParam->stringValue());
    };
}

void OSCController::processMessage(const OSCMessage & msg)
{
    if (logIncomingOSC->boolValue())
    {
        String log;
        log = msg.getAddressPattern().toString()+":";
        for(int i = 0 ; i < msg.size() ; i++){
            OSCArgument a = msg[i];
            if(a.isInt32())log+=String(msg[i].getInt32())+" ";
            else if(a.isFloat32())log+=String(msg[i].getFloat32())+" ";
            else if(a.isString())log+=String(msg[i].getString())+" ";

        }
        NLOG(niceName,log);

    }
	if (!enabledParam->boolValue()) return;
	bool result = processMessageInternal(msg);
	oscListeners.call(&OSCControllerListener::messageProcessed, msg, result);

	activityTrigger->trigger();
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
