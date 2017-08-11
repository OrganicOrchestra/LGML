/*
 ==============================================================================

 OSCController.cpp
 Created: 2 Mar 2016 8:50:08pm
 Author:  bkupe

 ==============================================================================
 */

#include "OSCController.h"

#include "DebugHelpers.h"

#include "NodeManager.h"

#include "NetworkUtils.h"



extern ThreadPool * getEngineThreadPool();


class OSCClientModel:public EnumParameterModel,NetworkUtils::Listener{

public:

  OSCClientModel(){
    addOption("localHost","127.0.0.1");
    NetworkUtils::getInstance()->addListener(this);
  };
  ~OSCClientModel(){
  }

  void oscClientAdded(OSCClientRecord &o) {
    addOrSetOption(o.getShortName(), o.ipAddress.toString());
  };
  void oscClientRemoved(OSCClientRecord &o) {
    removeOption(o.getShortName());
  };

};

OSCController::OSCController(const String &_name) :
Controller(_name),
lastMessageReceived(OSCAddressPattern("/fake")),
isProcessingOSC(false),
oscMessageQueue(this),
hostNameResolved(false),
isResolving(false)

{
  NetworkUtils::getInstance();
  localPortParam = addNewParameter<StringParameter>("Local Port", "The port to bind for the controller to receive OSC from it","11000");

  static OSCClientModel model;
  remoteHostParam = addNewParameter<EnumParameter>("Remote Host", "The host's IP of the remote controller",&model,true);
  remotePortParam = addNewParameter<StringParameter>("Remote Port", "The port bound by the controller to send OSC to it","8000");

  logIncomingOSC = addNewParameter<BoolParameter>("logIncomingOSC", "log the incoming OSC Messages", false);
  logOutGoingOSC = addNewParameter<BoolParameter>("logOutGoingOSC", "log the outGoing OSC Messages", false);
  speedLimit = addNewParameter<FloatParameter>("speedLimit", "min interval (ms) between 2 series of "+String(NUM_OSC_MSG_IN_A_ROW)+" OSCMessages", 0,0,100);

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
  //DBG("Resetup sender with " << remoteHostParam->stringValue() << ":" << remotePortParam->stringValue().getIntValue());
  if(isResolving) return;
  sender.disconnect();
  hostNameResolved = false;
  resolveHostnameIfNeeded(true);

  if(!isResolving && !hostNameResolved){
    LOG("no valid ip found for " << remoteHostParam->stringValue());
  }

}

class ResolveIPJob : public ThreadPoolJob{
  public :
  ResolveIPJob(OSCController* cont):owner(cont),ThreadPoolJob("resolveIP"){

  }
  WeakReference<ControllableContainer> owner;

  JobStatus runJob()override{
    
    if(OSCController* c = (OSCController*) owner.get()){

      OSCClientRecord resolved = NetworkUtils::hostnameToOSCRecord(c->remoteHostParam->stringValue());

      if(OSCController* c = (OSCController*) owner.get()){
        if(resolved.isValid()){
          c->hostNameResolved = true;
          c->remoteIP = resolved.ipAddress.toString();
          c->remotePortParam->setValue(String((int)resolved.port));
          LOG("resolved IP : "<<c->remoteHostParam->stringValue() << " > "<<c->remoteIP);
          c->sender.connect(c->remoteIP, c->remotePortParam->stringValue().getIntValue());
        }
        else{
          LOG("can't resolve IP : "<<c->remoteHostParam->stringValue() );
        }
        c->isResolving=false;
      }

    }
    return JobStatus::jobHasFinished;

  };

};

void OSCController::resolveHostnameIfNeeded(bool force){
  if(hostNameResolved) return;
  if(isResolving && !force) return;
  String hostName = remoteHostParam->stringValue();
  if(hostName.isNotEmpty()){
    getEngineThreadPool()->removeJob(resolveJob,true,-1);
    resolveJob = nullptr;
  if(!NetworkUtils::isValidIP(hostName)){
    isResolving = true;
    resolveJob = new ResolveIPJob(this);
    getEngineThreadPool()->addJob(resolveJob, true);

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
      tC->addChildControllableContainer(c );
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
  else if (p == remotePortParam || p == remoteHostParam) setupSender();
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
