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

#include "NodeManager.h"

OSCController::OSCController(const String &_name) :
Controller(_name),
lastMessageReceived(OSCAddressPattern("")),
isProcessingOSC(false)

{

  localPortParam = addStringParameter("Local Port", "The port to bind for the controller to receive OSC from it","11000");

  remoteHostParam = addStringParameter("Remote Host", "The host's IP of the remote controller","127.0.0.1");
  remotePortParam = addStringParameter("Remote Port", "The port bound by the controller to send OSC to it","8000");

  logIncomingOSC = addBoolParameter("logIncomingOSC", "log the incoming OSC Messages", false);
  logOutGoingOSC = addBoolParameter("logOutGoingOSC", "log the outGoing OSC Messages", false);

  blockFeedback = addBoolParameter("blockFeedback", "block osc feedback (resending updated message to controller)", true);
  sendAllParameters = addTrigger("sendAll", "send all parameter states to initialize ", true);
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
    logMessage(msg,"In:");

  }
  if (!enabledParam->boolValue()) return;

  if(blockFeedback->boolValue()){
    lastMessageReceived = msg;}
  isProcessingOSC = true;
  bool result = processMessageInternal(msg);
  isProcessingOSC = false;
  oscListeners.call(&OSCControllerListener::messageProcessed, msg, result);

  activityTrigger->trigger();
}


void OSCController::logMessage(const OSCMessage & msg,const String & prefix){
  String log = prefix;
  log += msg.getAddressPattern().toString()+":";
  for(int i = 0 ; i < msg.size() ; i++){
    OSCArgument a = msg[i];
    if(a.isInt32())log+=String(msg[i].getInt32())+" ";
    else if(a.isFloat32())log+=String(msg[i].getFloat32())+" ";
    else if(a.isString())log+=String(msg[i].getString())+" ";

  }
  NLOG(niceName,log);
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

void OSCController::onContainerTriggerTriggered(Trigger *t){
  Controller::onContainerTriggerTriggered(t);
   if(t==sendAllParameters){
    int sentCount = 0;
    sendAllControllableStates(NodeManager::getInstance(), sentCount);
  }
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


inline bool compareOSCArg(const OSCArgument & a, const OSCArgument & b){
  if(a.getType()!=b.getType()){
    return false;
  }

  if(a.getType()== OSCTypes::float32){
    return a.getFloat32()==b.getFloat32();
  }
  if(a.getType()== OSCTypes::string){
    return a.getString()==b.getString();
  }
  if(a.getType()== OSCTypes::int32){
    return a.getInt32()==b.getInt32();
  }
  if(a.getType()== OSCTypes::blob){
    return a.getBlob()==b.getBlob();
  }
  return false;
}

inline bool compareOSCMessages(const  OSCMessage & a,const OSCMessage & b){
  if(a.getAddressPattern()!=b.getAddressPattern()){
    return false;
  }
  if(a.size()!=b.size()){
    return false;
  }
  for(int i=0 ; i <a.size();i++){
    if(!compareOSCArg(a[i],b[i])){
      return false;
    }
  }
  return true;

}
bool OSCController::sendOSC (OSCMessage & m)
{
  if(enabledParam->boolValue() &&
     (!blockFeedback->boolValue() || !isProcessingOSC ||  !compareOSCMessages(lastMessageReceived,m))){
    if(logOutGoingOSC->boolValue()){
        logMessage(m,"Out:");
    }
    return sender.send (m);
  }

  return false;
}

ControllerUI * OSCController::createUI()
{
  return new OSCControllerUI(this);
}

void OSCController::sendAllControllableStates(ControllableContainer *c,int & sentControllable){
  if(c){
    for(auto & controllable:c->getAllControllables()){
      controllableFeedbackUpdate(c,controllable);
      sentControllable++;
      if((sentControllable%60)==0){
        Thread::sleep(10);
      }
    }
    for(auto & container:c->controllableContainers){
      sendAllControllableStates(container,sentControllable);

    }
  }
  
}
