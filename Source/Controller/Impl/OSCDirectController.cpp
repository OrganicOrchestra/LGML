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

extern ApplicationProperties *getAppProperties();
bool OSC_NON_BLOCKING=true; // will be overriden by settings
template<>
void ParameterContainer::OwnedFeedbackListener<OSCDirectController>::parameterFeedbackUpdate (ParameterContainer* /*originContainer*/, ParameterBase* c,ParameterBase::Listener * notifier){


    if (owner->enabledParam->boolValue() && (!owner->blockFeedback->boolValue() || notifier!=(Controller*)owner))
    {

        auto _owner = owner;
        auto f = [_owner,c](){
            _owner->sendOSCFromParam(c);
        };
        // avoid locking other threads
        if(!OSC_NON_BLOCKING || MessageManager::getInstance()->isThisTheMessageThread()){
            f();
        }
        else{
            MessageManager::callAsync(f);
        }
    }



}


OSCDirectController::OSCDirectController (StringRef name):
OSCController (name),
pSync(this)
{

    OSC_NON_BLOCKING = getAppProperties()->getUserSettings()->getBoolValue("deferControllerFB",false);
    sendTimeInfo = addNewParameter<BoolParameter> ("sendTimeInfo", "send time information", false);
    fullSync = addNewParameter<BoolParameter> ("syncAllParameters", "sync every parameter like stats/NodesUI/FastMap...(useful for test and hacking things around)", false);

    if(fullSync->boolValue()){
        dynamic_cast<ParameterContainer*>(ParameterContainer::getRoot(true))->addFeedbackListener(&pSync);
    }
    else{
        NodeManager::getInstance()->addFeedbackListener (&pSync);
        if (sendTimeInfo->boolValue())
        {
        TimeManager::getInstance()->addFeedbackListener (&pSync);
        }
    }
    userContainer.addFeedbackListener (&pSync);

}

OSCDirectController::~OSCDirectController()
{
    if (NodeManager* nm = NodeManager::getInstanceWithoutCreating())
        nm->removeFeedbackListener (&pSync);

    if (TimeManager* tm = TimeManager::getInstanceWithoutCreating())
        tm->removeFeedbackListener (&pSync);

    userContainer.removeFeedbackListener (&pSync);

}

Result OSCDirectController::processMessageInternal (const OSCMessage& msg)
{
    Result result = Result::ok();
    String addr = msg.getAddressPattern().toString();
    auto addrArray = OSCAddressToArray (addr);
    ControlAddressType ca;
    for(auto & s:addrArray){ca.add(s);}

    bool wasInUserParams = false;

    if (auto* up = ( ParameterBase*)userContainer.getControllableForAddress (ca))
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

        Controllable* cont = root->getControllableForAddress(ca);
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
            TimeManager::getInstance()->addFeedbackListener (&pSync);
        }
        else
        {
            TimeManager::getInstance()->removeFeedbackListener (&pSync);
        }

    }
    if(p==fullSync){
        if(fullSync->boolValue()){
            if(sendTimeInfo->boolValue())
                TimeManager::getInstance()->removeFeedbackListener(&pSync);
            NodeManager::getInstance()->removeFeedbackListener(&pSync);
             dynamic_cast<ParameterContainer*>(ParameterContainer::getRoot(true))->addFeedbackListener(&pSync);
        }
        else{
             dynamic_cast<ParameterContainer*>(ParameterContainer::getRoot(true))->removeFeedbackListener(&pSync);
            if(sendTimeInfo->boolValue())
                TimeManager::getInstance()->addFeedbackListener(&pSync);
            NodeManager::getInstance()->addFeedbackListener(&pSync);


        }
    }



};




