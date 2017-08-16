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


#include "OSCController.h"

#include "DebugHelpers.h"

#include "NodeManager.h"

#include "NetworkUtils.h"




class OSCClientModel:public EnumParameterModel,NetworkUtils::Listener{

public:

  OSCClientModel(){
    NetworkUtils::getInstance()->addListener(this);
  };
  ~OSCClientModel(){
  }

  void oscClientAdded(OSCClientRecord o) {
    addOrSetOption(o.getShortName(), o.ipAddress.toString());
  };
  void oscClientRemoved(OSCClientRecord o) {
    removeOption(o.getShortName());
  };

};

OSCController::OSCController(const String &_name) :
Controller(_name),
lastMessageReceived(OSCAddressPattern("/fake")),
isProcessingOSC(false),
oscMessageQueue(this),
hostNameResolved(false)

{
  NetworkUtils::getInstance();
  localPortParam = addNewParameter<StringParameter>("Local Port", "The port to bind for the controller to receive OSC from it","11000");


  remotePortParam = addNewParameter<StringParameter>("Remote Port", "The port bound by the controller to send OSC to it","8000");
  static OSCClientModel model;
  remoteHostParam = addNewParameter<EnumParameter>("Remote Host", "The host's IP of the remote controller",&model,true);
  remoteHostParam->selectId("localhost", true);
  logIncomingOSC = addNewParameter<BoolParameter>("logIncomingOSC", "log the incoming OSC Messages", false);
  logOutGoingOSC = addNewParameter<BoolParameter>("logOutGoingOSC", "log the outGoing OSC Messages", false);
  speedLimit = addNewParameter<FloatParameter>("speedLimit", "min interval (ms) between 2 series of "+String(NUM_OSC_MSG_IN_A_ROW)+" OSCMessages", 0.f,0.f,100.f);

  blockFeedback = addNewParameter<BoolParameter>("blockFeedback", "block osc feedback (resending updated message to controller)", true);
  sendAllParameters =  addNewParameter<Trigger>("sendAll", "send all parameter states to initialize ", true);

  autoAddParameter = addNewParameter<BoolParameter>("autoAddParam", "add new parameter for each recieved OSC message", false);
  setupReceiver();
  setupSender();

  receiver.addListener(this);
  lastOSCMessageSentTime = 0;
  numSentInARow=NUM_OSC_MSG_IN_A_ROW;
  


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

  sender.disconnect();
  hostNameResolved = false;
  resolveHostnameIfNeeded();

  if(!hostNameResolved){
    LOG("no valid ip found for " << remoteHostParam->stringValue());
  }

}



void OSCController::resolveHostnameIfNeeded(){
  if(hostNameResolved) return;
  
  String hostName = remoteHostParam->stringValue();
  if(hostName.isNotEmpty()){
  if(!NetworkUtils::isValidIP(hostName)){
    OSCClientRecord resolved = NetworkUtils::hostnameToOSCRecord(hostName);
    if(resolved.isValid()){
      hostNameResolved = true;
      remoteIP = resolved.ipAddress.toString();
      remotePortParam->setValue(String((int)resolved.port));
      LOG("resolved IP : "<<hostName << " > "<<remoteIP);
      sender.connect(remoteIP, remotePortParam->stringValue().getIntValue());
    }
    else{
      LOG("can't resolve IP : "<<hostName);
    }
  }
  else{
    remoteIP = hostName;
    sender.connect(remoteIP, remotePortParam->stringValue().getIntValue());
    hostNameResolved = true;
  }
  }

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
  if(autoAddParameter->boolValue()){
    MessageManager::getInstance()->callAsync([this,msg](){checkAndAddParameterIfNeeded(msg);});
  }
  bool result = processMessageInternal(msg);
  isProcessingOSC = false;
  oscListeners.call(&OSCControllerListener::messageProcessed, msg, result);

  activityTrigger->trigger();
}


bool OSCController::setParameterFromMessage(Parameter *c,const OSCMessage & msg,bool force){
  Controllable::Type targetType = c->type;
  if (targetType == Controllable::Type::PROXY) targetType = ((ParameterProxy *)c)->linkedParam->type;
  
  switch (targetType)
  {
    case Controllable::Type::TRIGGER:
      if (msg.size() == 0) ((Trigger *)c)->trigger();
      else if (msg[0].isInt32() || msg[0].isFloat32())
      {
        float val = msg[0].isInt32() ? msg[0].getInt32() : msg[0].getFloat32();
        if (val > 0) ((Trigger *)c)->trigger();
      }
      break;

    case Controllable::Type::BOOL:
      if (msg.size() > 0 && (msg[0].isInt32() || msg[0].isFloat32()))
      {
        float val = msg[0].isInt32() ? msg[0].getInt32() : msg[0].getFloat32();
        ((Parameter *)c)->setValue(val > 0,false,force);
      }
      break;

    case Controllable::Type::FLOAT:
      if (msg.size() > 0 && (msg[0].isInt32() || msg[0].isFloat32()))
      {
        float value = msg[0].isInt32() ? msg[0].getInt32() : msg[0].getFloat32();
        ((Parameter *)c)->setValue((float)value,false,force); //normalized or not ? can user decide ?
      }
      break;

    case Controllable::Type::INT:
      if (msg.size() > 0 && (msg[0].isInt32() || msg[0].isFloat32()))
      {
        int value = msg[0].isInt32() ? msg[0].getInt32() : (int)msg[0].getFloat32();
        ((Parameter *)c)->setValue(value,false,force);
      }
      break;

    case Controllable::Type::STRING:
      if (msg.size() > 0){
        // cast number to strings
        if  (msg[0].isInt32() || msg[0].isFloat32()){
          float value = msg[0].isInt32() ? msg[0].getInt32() : msg[0].getFloat32();
          ((Parameter *)c)->setValue(String(value));
        }
        else if (msg[0].isString()){
          ((Parameter *)c)->setValue(msg[0].getString(),false,force);
        }
      }
      break;

    case Controllable::Type::ENUM:
      if (msg.size() > 0){
        // cast float to int
        if  (msg[0].isInt32() || msg[0].isFloat32()){
          int value = msg[0].isInt32() ? msg[0].getInt32() : msg[0].getFloat32();
          ((Parameter *)c)->setValue(value,false,force);
        }
        // select by name
        else if (msg[0].isString()){
          ((Parameter *)c)->setValue(msg[0].getString(),false,force);
        }
      }
      break;



    default:
      return false;
      
      
  }
  return true;
}


void OSCController::checkAndAddParameterIfNeeded(const OSCMessage & msg){

  // TODO handle wildcards
  String addr = msg.getAddressPattern().toString();
  auto * linked = userContainer.getControllableForAddress(addr);
  if(!linked){

    StringArray sa =OSCAddressToArray(addr);
    ControllableContainer * tC = &userContainer;
    for( int i = 0 ; i < sa.size()-1 ; i++){
    auto *c = tC->getControllableContainerByName(sa[i],true);
    if(!c){
      c = new ControllableContainer(sa[i],true);
      tC->addChildControllableContainer(c ,false);
    }
      tC = c;
    }

    String pName = sa[sa.size()-1];
    if(tC){
    if(msg.size()==0){
      linked = tC->addNewParameter<Trigger>(pName, "entry for "+msg.getAddressPattern().toString());
    }
    else{
      if(msg[0].isString()){
        linked = tC->addNewParameter<StringParameter>(pName, "entry for "+msg.getAddressPattern().toString());
      }
      else if(msg[0].isInt32()){
        linked = tC->addNewParameter<IntParameter>(pName, "entry for "+msg.getAddressPattern().toString());
      }
      else if(msg[0].isFloat32()){
        linked = tC->addNewParameter<FloatParameter>(pName, "entry for "+msg.getAddressPattern().toString());
      }
    }

      if(auto *p = linked->getParameter()){
        setParameterFromMessage(p, msg,true);
      }
    }
    else{
      jassertfalse;
    }

  }




}

void OSCController::logMessage(const OSCMessage & msg,const String & prefix){
  String log = prefix;
  log += msg.getAddressPattern().toString()+":";
  for(int i = 0 ; i < msg.size() ; i++){
    OSCArgument a = msg[i];
    if(a.isInt32())log+=String(msg[i].getInt32())+" ";
    else if(a.isFloat32())log+=String(msg[i].getFloat32(),2)+" ";
    else if(a.isString())log+=String(msg[i].getString())+" ";

  }
  NLOG(getNiceName(),log);
}

Result OSCController::processMessageInternal(const OSCMessage &)
{
  return Result::fail("Not handled"); //if not overriden, msg is not handled so result is false
}

void OSCController::onContainerParameterChanged(Parameter * p)
{
  Controller::onContainerParameterChanged(p);

  if (p == localPortParam) setupReceiver();
  else if ((p == remotePortParam&& !remoteHostParam->isSettingValue)||p == remoteHostParam ) setupSender();
  else if(p==speedLimit){oscMessageQueue.interval=speedLimit->floatValue();}



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
  if(enabledParam->boolValue() ){
    resolveHostnameIfNeeded();
    if(hostNameResolved){
      if(!blockFeedback->boolValue() ||   !compareOSCMessages(lastMessageReceived,m)){//!isProcessingOSC ||

        if(speedLimit->floatValue()>0.0f){
          oscMessageQueue.add(new OSCMessage(m));
        }
        else{
          return sendOSCInternal(m);
        }

      }
    }
  }

  return false;
}
bool OSCController::sendOSCInternal(OSCMessage & m){
  if(logOutGoingOSC->boolValue()){ logMessage(m,"Out:");}
  return sender.send (m);
}



void OSCController::sendAllControllableStates(ControllableContainer *c,int & sentControllable){
  if(c){
    for(auto & controllable:c->getAllControllables()){
      controllableFeedbackUpdate(c,controllable);
      sentControllable++;
      if((sentControllable%10)==0){
        Thread::sleep(2);
      }
    }
    for(auto & container:c->controllableContainers){
      sendAllControllableStates(container,sentControllable);
    }
  }

}


////////////////////////
// OSCMessageQueue
///////////////////////
OSCController::OSCMessageQueue::OSCMessageQueue(OSCController* o):
owner(o),
aFifo(OSC_QUEUE_LENGTH),
interval(1)
{messages.resize(OSC_QUEUE_LENGTH);}

void OSCController::OSCMessageQueue::add(OSCMessage * m){
  int startIndex1,blockSize1,startIndex2,blockSize2;
  aFifo.prepareToWrite(1,startIndex1,blockSize1,startIndex2,blockSize2);
  int numWritten = 0;
  if(blockSize1>0){
    messages.set(startIndex1,m);
    numWritten ++;
  }
  else if(blockSize2>0){
    messages.set(startIndex2,m);
    numWritten ++;
  }
  else{
    aFifo.finishedWrite(numWritten);
    numWritten=0;
    timerCallback();
    NLOG(owner->getNiceName(),"still flooding OSC");
    delete m;
  }
  aFifo.finishedWrite(numWritten);
  if(!isTimerRunning())startTimer(interval);
}

void OSCController::OSCMessageQueue::timerCallback() {
  if(aFifo.getNumReady()){
    int numRead=0;
    int startIndex1,blockSize1,startIndex2,blockSize2;
    aFifo.prepareToRead(NUM_OSC_MSG_IN_A_ROW, startIndex1,blockSize1,startIndex2,blockSize2);
    if(blockSize1>0){
      for( ; numRead < blockSize1 ; numRead++ ){
        owner->sendOSCInternal(*messages[startIndex1+numRead]);
        delete messages[startIndex1+numRead];
      }
    }
    if(blockSize2>0){
      for(int i = 0 ; i < blockSize2 ; i++ ){
        owner->sendOSCInternal(*messages[startIndex2+i]);
        delete messages[startIndex2+i];
        numRead++;
      }
    }
    aFifo.finishedRead(numRead);
    
  }
  else{
    stopTimer();
  }
}

StringArray OSCController::OSCAddressToArray(const String & addr){
  StringArray addrArray;
  addrArray.addTokens(addr,juce::StringRef("/"), juce::StringRef("\""));
  addrArray.remove(0);
  return addrArray;
}
