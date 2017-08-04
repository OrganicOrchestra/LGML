/*
 ==============================================================================

 Controller.cpp
 Created: 2 Mar 2016 8:49:50pm
 Author:  bkupe

 ==============================================================================
 */

#include "Controller.h"
#include "ControllerFactory.h"
#include "ControllerUI.h"
#include "ControllerEditor.h"
#include "ControlManager.h"
#include "DebugHelpers.h"


const Identifier Controller::controllerTypeIdentifier("controllerType");




Controller::Controller(const String &_name) :
ControllableContainer(_name),
userContainer("messages",true)
{
  addChildControllableContainer(&userContainer);
  userContainer.nameParam->isEditable = false;

  enabledParam = addNewParameter<BoolParameter>("Enabled","Set whether the controller is enabled or disabled", true);

  activityTrigger =  addNewParameter<Trigger>("activity", "Activity indicator");
  activityTrigger->isEditable = false;
  controllerTypeEnum = 0; //init
}


Controller::~Controller()
{
  if(parentContainer){
    parentContainer->removeChildControllableContainer(this);
  }


  //DBG("Remove Controller");
}


var Controller::getJSONData(){

  var res = ControllableContainer::getJSONData();
  res.getDynamicObject()->setProperty(controllerTypeIdentifier,
                                      ControllerFactory::controllerTypeToString((ControllerFactory::ControllerType)controllerTypeEnum));
  return res;


}
ControllerUI * Controller::createUI(){return new ControllerUI(this);}

ControllerEditor * Controller::createEditor(){return new ControllerEditor(this);}

void Controller::remove(){
  ((ControllerManager*)parentContainer)->removeController(this);
}

void Controller::onContainerParameterChanged(Parameter * p)
{
  if (p == nameParam)
  {
    setNiceName(nameParam->stringValue());
  }
  else if (p == enabledParam)
  {
    if(JsEnvironment * jsEnv = dynamic_cast<JsEnvironment*>(this)){

      jsEnv->setEnabled(enabledParam->boolValue());
    }

    // DBG("set Controller Enabled " + String(enabledParam->boolValue()));
  }
}

void Controller::onContainerTriggerTriggered(Trigger *){}

