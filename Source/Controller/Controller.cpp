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


#include "Controller.h"
#include "ControllerFactory.h"
#include "UI/ControllerUI.h"
#include "UI/ControllerEditor.h"
#include "ControllerManager.h"
#include "../Utils/DebugHelpers.h"


const Identifier Controller::controllerTypeIdentifier("controllerType");




Controller::Controller(const String &_name) :
ParameterContainer(_name),
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

  var res = ParameterContainer::getJSONData();
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
