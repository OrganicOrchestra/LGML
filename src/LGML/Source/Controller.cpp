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

#include "DebugHelpers.h"


const Identifier Controller::controllerTypeIdentifier("controllerType");
const Identifier Controller::variableNameIdentifier("name");
const Identifier Controller::variableMinIdentifier("min");
const Identifier Controller::variableMaxIdentifier("max");

Controller::Controller(const String &_name) :
ControllableContainer(_name)
{

  enabledParam = addNewParameter<BoolParameter>("Enabled","Set whether the controller is enabled or disabled", true);

  activityTrigger =  addNewParameter<Trigger>("activity", "Activity indicator");

  controllerTypeEnum = 0; //init
}


Controller::~Controller()
{
  //DBG("Remove Controller");
}






ControllerUI * Controller::createUI(){return new ControllerUI(this);}

ControllerEditor * Controller::createEditor(){return new ControllerEditor(this);}


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

