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

#define NON_BLOCKING 0
template<>
void ControllableContainer::OwnedFeedbackListener<OSCDirectController>::controllableFeedbackUpdate (ControllableContainer* originContainer, Controllable* c){


    if (owner->enabledParam->boolValue())
    {
#if NON_BLOCKING
        auto f = [this,c](){
#endif
            owner->sendOSCFromParam(c);
#if NON_BLOCKING
        };
        // avoid locking other threads
        if(MessageManager::getInstance()->isThisTheMessageThread()){
            f();
        }
        else{
            MessageManager::callAsync(f);
        }
#endif
    }



}


OSCDirectController::OSCDirectController (StringRef name):
OSCController (name),
pSync(this)
{


    sendTimeInfo = addNewParameter<BoolParameter> ("sendTimeInfo", "send time information", false);
    fullSync = addNewParameter<BoolParameter> ("syncAllParameters", "sync every parameter like stats/NodesUI/FastMap...(useful for test and hacking things around)", false);

    if(fullSync->boolValue()){
        ParameterContainer::getRoot(true)->addControllableContainerListener(&pSync);
    }
    else{
        NodeManager::getInstance()->addControllableContainerListener (&pSync);
        if (sendTimeInfo->boolValue())
        {
        TimeManager::getInstance()->addControllableContainerListener (&pSync);
        }
    }

}

OSCDirectController::~OSCDirectController()
{
    if (NodeManager* nm = NodeManager::getInstanceWithoutCreating())
        nm->removeControllableContainerListener (&pSync);

    if (TimeManager* tm = TimeManager::getInstanceWithoutCreating())
        tm->removeControllableContainerListener (&pSync);

}

Result OSCDirectController::processMessageInternal (const OSCMessage& msg)
{
    Result result = Result::ok();
    String addr = msg.getAddressPattern().toString();
    auto addrArray = OSCAddressToArray (addr);

    bool wasInUserParams = false;

    if (auto* up = ( ParameterBase*)userContainer.getControllableForAddress (addrArray))
    {
        if (!setParameterFromMessage (up, msg))
        {
            result =  Result::fail ("Controllable type not handled in user Parameter");
        }
        else{
            wasInUserParams=true;
        }
    }
    auto root = ParameterContainer::getRoot(true);

    if(msg.getAddressPattern().containsWildcards()){



        auto params = root->getControllablesForExtendedAddress(addrArray);
        for(auto cont:params){
            if (auto c = ParameterBase::fromControllable (cont))
            {
                if (c->isControllableExposed && c->isEditable)
                {
                    if (!setParameterFromMessage (c, msg,false,false) )
                    {
                        result = Result::fail ("Controllable type not handled");
                    }
                }
            }
        }
    }
    else{

        


        Controllable* cont = root->getControllableForAddress(addrArray);



        if (auto c = ParameterBase::fromControllable (cont))
        {
            if (c->isControllableExposed && c->isEditable)
            {
                if (!setParameterFromMessage (c, msg))
                {
                    result = Result::fail ("Controllable type not handled");
                }
            }
        }
        else if( !wasInUserParams)
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


void OSCDirectController::onContainerParameterChanged ( ParameterBase* p)
{
    OSCController::onContainerParameterChanged (p);

    if (p == sendTimeInfo)
    {
        if (sendTimeInfo->boolValue())
        {
            TimeManager::getInstance()->addControllableContainerListener (&pSync);
        }
        else
        {
            TimeManager::getInstance()->removeControllableContainerListener (&pSync);
        }

    }
    if(p==fullSync){
        if(fullSync->boolValue()){
            if(sendTimeInfo->boolValue())
                TimeManager::getInstance()->removeControllableContainerListener(&pSync);
            NodeManager::getInstance()->removeControllableContainerListener(&pSync);
            ParameterContainer::getRoot(true)->addControllableContainerListener(&pSync);
        }
        else{
            ParameterContainer::getRoot(true)->removeControllableContainerListener(&pSync);
            if(sendTimeInfo->boolValue())
                TimeManager::getInstance()->addControllableContainerListener(&pSync);
            NodeManager::getInstance()->addControllableContainerListener(&pSync);


        }
    }



};




