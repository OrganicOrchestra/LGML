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


#include "OSCDirectController.h"
#include "../../Node/Manager/NodeManager.h"
#include "../../Utils/DebugHelpers.h"
#include "../../Time/TimeManager.h"
#include "../../Controllable/Parameter/ParameterProxy.h"
#include "../ControllerManager.h"



//REGISTER_OBJ_TYPE_NAMED (Controller, OSCDirectController, "t_OSC");

OSCDirectController::OSCDirectController (StringRef name):
OSCController (name)
{


    sendTimeInfo = addNewParameter<BoolParameter> ("sendTimeInfo", "send time information", false);
    fullSync = addNewParameter<BoolParameter> ("syncAllParameters", "sync every parameter", false);

    if(fullSync->boolValue()){
        ParameterContainer::getRoot(true)->addControllableContainerListener(this);
    }
    else{
        NodeManager::getInstance()->addControllableContainerListener (this);
        if (sendTimeInfo->boolValue())
        {
        TimeManager::getInstance()->addControllableContainerListener (this);
        }
    }

}

OSCDirectController::~OSCDirectController()
{
    if (NodeManager* nm = NodeManager::getInstanceWithoutCreating())nm->removeControllableContainerListener (this);

    if (TimeManager* tm = TimeManager::getInstanceWithoutCreating()) {tm->removeControllableContainerListener (this);}

}

Result OSCDirectController::processMessageInternal (const OSCMessage& msg)
{
    Result result = Result::ok();
    String addr = msg.getAddressPattern().toString();
    auto addrArray = OSCAddressToArray (addr);



    if (auto* up = (Parameter*)userContainer.getControllableForAddress (addrArray))
    {
        if (!setParameterFromMessage (up, msg))
        {
            result =  Result::fail ("Controllable type not handled in user Parameter");
        }
    }
    auto root = ParameterContainer::getRoot(true);

    if(msg.getAddressPattern().containsWildcards()){



        auto params = root->getControllablesForExtendedAddress(addrArray);
        for(auto cont:params){
            if (auto c = Parameter::fromControllable (cont))
            {
                if (c->isControllableExposed && c->isEditable)
                {
                    if (!setParameterFromMessage (c, msg,false,false))
                    {
                        result = Result::fail ("Controllable type not handled");
                    }
                }
            }
        }
    }
    else{

        


        Controllable* cont = root->getControllableForAddress(addrArray);



        if (auto c = Parameter::fromControllable (cont))
        {
            if (c->isControllableExposed && c->isEditable)
            {
                if (!setParameterFromMessage (c, msg))
                {
                    result = Result::fail ("Controllable type not handled");
                }
            }
        }
        else
        {
            result = Result::fail ("Controllable not found");

            DBG ("No Controllable for address : " + addr);
        }

    }

    return result;
}




String getValidOSCAddress (const String& s)
{
    String targetName = s;

    if (!s.startsWithChar ('/'))
    {
        targetName = '/' + s;
    }

    targetName.replace (" ", "");

    return targetName;
}

void OSCDirectController::controllableAdded (ControllableContainer*, Controllable* )  {}
void OSCDirectController::controllableRemoved (ControllableContainer*, Controllable*) {}

void OSCDirectController::onContainerParameterChanged (Parameter* p)
{
    OSCController::onContainerParameterChanged (p);

    if (p == sendTimeInfo)
    {
        if (sendTimeInfo->boolValue())
        {
            TimeManager::getInstance()->addControllableContainerListener (this);
        }
        else
        {
            TimeManager::getInstance()->removeControllableContainerListener (this);
        }

    }
    if(p==fullSync){
        if(fullSync->boolValue()){
            if(sendTimeInfo->boolValue())
                TimeManager::getInstance()->removeControllableContainerListener(this);
            NodeManager::getInstance()->removeControllableContainerListener(this);
            ParameterContainer::getRoot(true)->addControllableContainerListener(this);
        }
        else{
            ParameterContainer::getRoot(true)->removeControllableContainerListener(this);
            if(sendTimeInfo->boolValue())
                TimeManager::getInstance()->addControllableContainerListener(this);
            NodeManager::getInstance()->addControllableContainerListener(this);


        }
    }



};

void OSCDirectController::sendOSCForAddress (Controllable* c, const String& cAddress)
{


    if (Parameter* p = Parameter::fromControllable (c))
    {
        auto  targetType = p->getFactoryTypeId();

        if (targetType == ParameterProxy::_factoryType) targetType = ((ParameterProxy*)c)->linkedParam->getFactoryTypeId();

        if (targetType == Trigger::_factoryType) {sendOSC (cAddress);}
        else if (targetType == BoolParameter::_factoryType) {sendOSC (cAddress, p->intValue());}
        else if (targetType == FloatParameter::_factoryType) {sendOSC (cAddress, p->floatValue());}
        else if (targetType == IntParameter::_factoryType) {sendOSC (cAddress, p->intValue());}
        else if (targetType == StringParameter::_factoryType) {sendOSC (cAddress, p->stringValue());}
        else if (targetType == EnumParameter::_factoryType) {sendOSC (cAddress, p->stringValue());}
        else
        {
            DBG ("Type not supported " << targetType.toString());
            jassertfalse;
        }

    }
    else
    {
        jassertfalse;
    }
}
void OSCDirectController::controllableFeedbackUpdate (ControllableContainer* /*originContainer*/, Controllable* c)
{

    if (enabledParam->boolValue())
    {
        if (c->isChildOf (&userContainer))
        {
            sendOSCForAddress (c, c->getControlAddress (&userContainer));
        }
        else
        {
            sendOSCForAddress (c, c->controlAddress);
        }
    }


}

void OSCDirectController::controllableContainerAdded (ControllableContainer*, ControllableContainer*)
{
}

void OSCDirectController::controllableContainerRemoved (ControllableContainer*, ControllableContainer*)
{
}
