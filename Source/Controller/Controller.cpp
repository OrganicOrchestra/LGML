/*
 ==============================================================================

 Copyright © Organic Orchestra, 2017

 This file is part of LGML. LGML is a software to manipulate sound in real-time

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
    userContainer ("messages"),
    autoAddParams(false)
{
    userContainer.setUserDefined (true);
    userContainer.addControllableContainerListener (this);
    addChildControllableContainer (&userContainer);
    userContainer.nameParam->setInternalOnlyFlags(true,true);

    enabledParam = addNewParameter<BoolParameter> ("Enabled", "Set whether the controller is enabled or disabled", true);

    isConnected = addNewParameter<BoolParameter> ("Connected", "status of Controller connection", false);
    isConnected->setInternalOnlyFlags(true,false);


    inActivityTrigger =  addNewParameter<Trigger> ("in activity", "In Activity indicator");
    outActivityTrigger =  addNewParameter<Trigger> ("out activity", "Out Activity indicator");
    inActivityTrigger->setInternalOnlyFlags(true,false);

    outActivityTrigger->setInternalOnlyFlags(true,false);
    
    controllerTypeEnum = 0; //init
}


Controller::~Controller()
{
    if (parentContainer)
    {
        parentContainer->removeChildControllableContainer (this);
    }
    Controller::masterReference.clear();

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

void Controller::onContainerParameterChanged ( ParameterBase* p)
{
    ParameterContainer::onContainerParameterChanged(p);

    if (p == enabledParam)
    {
        //TODO move this behaviour to jsEnv
        if (JsEnvironment* jsEnv = dynamic_cast<JsEnvironment*> (this))
        {

            jsEnv->setScriptEnabled (enabledParam->boolValue());
        }

        // DBG("set Controller Enabled " + String(enabledParam->boolValue()));
    }
}

void Controller::setMappingMode(bool state) {
    autoAddParams = state;
};

void Controller::onContainerTriggerTriggered (Trigger*) {}
