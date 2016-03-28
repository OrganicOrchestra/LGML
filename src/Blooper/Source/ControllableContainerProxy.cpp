/*
 ==============================================================================

 ControllableContainerProxy.cpp
 Created: 27 Mar 2016 3:25:47pm
 Author:  Martin Hermant

 ==============================================================================
 */

#include "ControllableContainerProxy.h"


void ControllableContainerProxy::addProxyListener(ControllableContainer * c){
    // check that we have same class
    jassert(typeid(c)==typeid(sourceContainer));
    c->addControllableContainerListener(this);
    proxyControllableListeners.add(c);
}
void ControllableContainerProxy::removeProxyListener(ControllableContainer * c){
    // if source is deleted use another listener as source
    c->removeControllableContainerListener(this);

    if(c==sourceContainer){

        if(proxyControllableListeners.size()>0){
            buildFromContainer(proxyControllableListeners.getUnchecked(0));
        }
        else{
            sourceContainer = nullptr;
        }
    }
    else{
        proxyControllableListeners.removeFirstMatchingValue(c);

    }
}


void ControllableContainerProxy::controllableFeedbackUpdate(Controllable *cOrigin){
    String addr =  cOrigin->getControlAddress();

    StringArray addrArray;
    addrArray.addTokens(addr,juce::StringRef("/"), juce::StringRef("\""));
    juce::Array<String> addSplit = addrArray.strings;

    for(int i = 0 ; i < depthInOriginContainer ; i ++){
        addSplit.remove(0);
    }

    String controller = addSplit[0];
    addSplit.remove(0);
    for(auto & listener:proxyControllableListeners){
        Controllable * c = listener->getControllableForAddress(addSplit,true,true);
        //block feedback loop ?
        //        if(c==cOrigin)return;

        jassert(cOrigin->type == c->type);

        if (c != nullptr && !c->isControllableFeedbackOnly)
        {


            switch (c->type)
            {
                case Controllable::Type::TRIGGER:
                    ((Trigger *)c)->trigger();
                    break;

                case Controllable::Type::BOOL:
                    ((BoolParameter *)c)->setValue(((BoolParameter*)cOrigin)->value);
                    break;

                case Controllable::Type::FLOAT:
                    ((FloatParameter *)c)->setValue(((FloatParameter*)cOrigin)->value);
                    break;

                case Controllable::Type::INT:
                    ((IntParameter *)c)->setValue(((IntParameter*)cOrigin)->value);
                    break;

                case Controllable::Type::RANGE:
                    DBG("float range not supported");
                    jassert(false);
                    //                    ((FloatRangeParameter *)c)->setValue(((FloatRangeParameter*)cOrigin)->value);
                    break;

                case Controllable::Type::STRING:
                    ((StringParameter *)c)->setValue(((StringParameter*)cOrigin)->value);
                    break;
                default:
                    jassertfalse;
            }
        }
        else
        {
            DBG("No Controllable for address : " + addr);
        }
    }
}
