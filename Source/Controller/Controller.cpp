/*
 ==============================================================================

 Copyright © Organic Orchestra, 2017

 This file is part of LGML. LGML is a software to manipulate sound in realtime

 This program is free software; you can redistribute it and/or modify
 it under the terms of the GNU General Public License as published by
 the Free Software Foundation (version 3 of the License).

 This program is distributed in the hope that it will be useful,
 but WITHOUT ANY WARRANTY; without even the implied warranty of
 MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.

 ==============================================================================
 */

#include "Controller.h"
#include "ControllerFactory.h"
#include "ControllerManager.h"
#include "../Utils/DebugHelpers.h"
#include "../Scripting/Js/JsEnvironment.h"


Controller::Controller (StringRef _name) :
    ParameterContainer (_name),
    userContainer ("messages")
{
    userContainer.setUserDefined (true);
    addChildControllableContainer (&userContainer);
    userContainer.nameParam->isEditable = false;

    enabledParam = addNewParameter<BoolParameter> ("Enabled", "Set whether the controller is enabled or disabled", true);

    inActivityTrigger =  addNewParameter<Trigger> ("in activity", "In Activity indicator");
    outActivityTrigger =  addNewParameter<Trigger> ("out activity", "Out Activity indicator");
    inActivityTrigger->isEditable = false;
    inActivityTrigger->isControllableExposed =false;
    outActivityTrigger->isEditable = false;
    outActivityTrigger->isControllableExposed =false;
    controllerTypeEnum = 0; //init
}


Controller::~Controller()
{
    if (parentContainer)
    {
        parentContainer->removeChildControllableContainer (this);
    }


    //DBG("Remove Controller");
}



void Controller::remove()
{
    if(parentContainer){
        ((ControllerManager*)parentContainer)->removeController (this);
    }
    else{
        jassertfalse;
    }
}

void Controller::onContainerParameterChanged (Parameter* p)
{
    if (p == nameParam)
    {
        setNiceName (nameParam->stringValue());
    }
    else if (p == enabledParam)
    {
#warning move this behaviour to jsEnv
        if (JsEnvironment* jsEnv = dynamic_cast<JsEnvironment*> (this))
        {

            jsEnv->setEnabled (enabledParam->boolValue());
        }

        // DBG("set Controller Enabled " + String(enabledParam->boolValue()));
    }
}

void Controller::onContainerTriggerTriggered (Trigger*) {}
