/*
 ==============================================================================

 SerialController.cpp
 Created: 22 May 2016 4:50:41pm
 Author:  bkupe

 ==============================================================================
 */

#include "SerialController.h"
#include "SerialControllerUI.h"
#include "JsHelpers.h"

SerialController::SerialController() :
JsEnvironment("controller.serial",this),
Controller("Serial"),
port(nullptr)
{

  setNamespaceName("controller." + shortName);
  logIncoming = addBoolParameter("logIncoming", "log Incoming midi message", false);

  selectedHardwareID = addStringParameter("selectedHardwareID","Id of the selected hardware", "");
  selectedPort = addStringParameter("selectedPort","Name of the selected hardware", "");

  SerialManager::getInstance()->addSerialManagerListener(this);
}

SerialController::~SerialController()
{
  if (SerialManager::getInstanceWithoutCreating() != nullptr)
  {
    SerialManager::getInstance()->removeSerialManagerListener(this);
  }

  setCurrentPort(nullptr);
}

void SerialController::setCurrentPort(SerialPort * _port)
{

  if (port == _port) return;


  if (port != nullptr)
  {

    port->removeSerialPortListener(this);
  }

  port = _port;

  if (port != nullptr)
  {
    port->addSerialPortListener(this);
    lastOpenedPortID = port->info->port;

    selectedPort->setValue(port->info->port);
    selectedHardwareID->setValue(port->info->hardwareID);

    sendIdentificationQuery();
  } else
  {
    DBG("set port null");
  }

  DBG("current port changed");
  serialControllerListeners.call(&SerialControllerListener::currentPortChanged);
}

void SerialController::newJsFileLoaded()
{

}

void SerialController::onContainerParameterChanged(Parameter * p) {
  Controller::onContainerParameterChanged(p);
  if(p==nameParam){
    setNamespaceName("controller."+shortName);
  }
  else if(p == selectedHardwareID || p == selectedPort)
  {
    SerialPort * _port  = SerialManager::getInstance()->getPort(selectedHardwareID->stringValue(), selectedPort->stringValue(),true);
    if(_port != nullptr)
    {
      setCurrentPort(_port);
    }
  }


};

void SerialController::buildLocalEnv() {

  DynamicObject obj;
  static const Identifier jsSendMessageIdentifier("sendMessage");
  obj.setMethod(jsSendMessageIdentifier, sendMessageFromScript);
  obj.setProperty(jsPtrIdentifier, (int64)this);

  for (auto &v : variables)
  {
    if(Parameter * p = v->parameter){
      obj.setProperty(v->parameter->shortName, p->createDynamicObject());
    }
  }

  setLocalNamespace(obj);
}

void SerialController::portOpened(SerialPort * )
{
  serialControllerListeners.call(&SerialControllerListener::portOpened);

  sendIdentificationQuery();
}

void SerialController::portClosed(SerialPort *)
{
  serialControllerListeners.call(&SerialControllerListener::portClosed);
}

void SerialController::portRemoved(SerialPort *)
{
  setCurrentPort(nullptr);
}

void SerialController::serialDataReceived(const var & data)
{
  activityTrigger->trigger();
  processMessage(data.toString());
}

void SerialController::internalVariableAdded(ControlVariable*){
  reloadFile();
}
void SerialController::internalVariableRemoved(ControlVariable*){
  reloadFile();

}
var SerialController::sendMessageFromScript(const var::NativeFunctionArgs &) {
  //    SerialController * c = getObjectPtrFromJS<SerialController>(a);
  return var::undefined();
}

void SerialController::sendIdentificationQuery()
{
  port->writeString("i");
}

void SerialController::processMessage(const String & message)
{
  StringArray split;
  split.addTokens(message.removeCharacters("\n"),true);
  String command = split[0];
  if (command == "i")
  {
    //identification
    deviceID = split[1];
    while (serialVariables.size() > 0)
    {
      removeVariable(serialVariables[0]);
      serialVariables.removeAllInstancesOf(serialVariables[0]);
    }

  } else if (command == "a")
  {
    if (getVariableForName(split[1]) == nullptr)
    {
      ControlVariable * v ;
      if(split.size()>=4){
        v= addVariable(new FloatParameter(split[1],split[1],split[2].getFloatValue(),split[2].getFloatValue(),split[3].getFloatValue()));
      }
      else{
        v = addVariable(new FloatParameter(split[1], split[1], 0));
      }
      serialVariables.add(v);
    }
  } else if (command == "d")
  {
    if (getVariableForName(split[1]) == nullptr)
    {
      ControlVariable * v = addVariable(new BoolParameter(split[1], split[1], false));
      serialVariables.add(v);
    }
  } else if (command == "u")
  {
    ControlVariable *  v = getVariableForName(split[1]);
    if (v != nullptr)
    {
      v->parameter->setValue(split[2].getFloatValue());
    }
  }
}

ControllerUI * SerialController::createUI()
{
  return new SerialControllerUI(this);
}

void SerialController::portAdded(SerialPortInfo * info)
{
  //DBG("SerialController, portAdded >" << info->hardwareID << "< > " << lastOpenedPortID);
  if (port == nullptr && lastOpenedPortID == info->port)
  {
    setCurrentPort(SerialManager::getInstance()->getPort(info));
  }
}

void SerialController::portRemoved(SerialPortInfo *)
{
}
