/*
 ==============================================================================

 ControllableContainerSync.cpp
 Created: 27 Mar 2016 3:25:47pm
 Author:  Martin Hermant

 ==============================================================================
 */

#include "ControllableContainerSync.h"


void ControllableContainerSync::addSyncedControllableIfNotAlreadyThere(ControllableContainer * c){
    // check that we have same class
    jassert(typeid(c)==typeid(sourceContainer));
    if(c!=sourceContainer && !targetSyncedContainers.contains(c)){
        c->addControllableContainerListener(this);
        targetSyncedContainers.add(c);
    }
}
void ControllableContainerSync::removeSyncedControllable(ControllableContainer * c){
    c->removeControllableContainerListener(this);

    // if removing source try to build from first available
    if(c==sourceContainer){

        if(targetSyncedContainers.size()>0){
            ControllableContainer * _c = targetSyncedContainers.getUnchecked(0);
            buildFromContainer(_c);
            containerSyncListeners.call(&ControllableContainerSync::ContainerSyncListener::sourceUpdated,_c);
        }
        else{
            sourceContainer = nullptr;
            containerSyncListeners.call(&ControllableContainerSync::ContainerSyncListener::sourceDeleted);

        }
    }
    else{
        targetSyncedContainers.removeFirstMatchingValue(c);

    }
}


void ControllableContainerSync::buildFromContainer(ControllableContainer * source){
    if(sourceContainer){
        sourceContainer->removeControllableContainerListener(this);
    }
    sourceContainer = source;
    sourceContainer->addControllableContainerListener(this);
    depthInOriginContainer = 0;
    ControllableContainer * t = sourceContainer;
    while(t!=nullptr){
        t = t->parentContainer;
        depthInOriginContainer++;
    }

};


// ===============================
// sync controllables

void ControllableContainerSync::controllableFeedbackUpdate(Controllable *cOrigin){
    if(isNotifying)return;
    isNotifying = true;
    String addr =  cOrigin->getControlAddress();

    StringArray addrArray;
    addrArray.addTokens(addr,juce::StringRef("/"), juce::StringRef("\""));
    addrArray.remove(0);
    juce::Array<String> addSplit = addrArray.strings;
    if(depthInOriginContainer+1> addSplit.size()){
        // coming from another controllableContainer with a shorter address
        return;
    }
    for(int i = 0 ; i < depthInOriginContainer ; i ++){
        addSplit.remove(0);
    }

    for(auto & listener:targetSyncedContainers){
        Controllable * c = listener->getControllableForAddress(addSplit,true,true);
        if(c!=nullptr)setControllableValue(cOrigin, c);
    }

    Controllable * c = sourceContainer->getControllableForAddress(addSplit,true,true);
    if(c!=cOrigin && c!=nullptr){
        setControllableValue(cOrigin, c);
    }
    isNotifying = false;
}


bool ControllableContainerSync::setControllableValue(Controllable * cOrigin,Controllable * c){
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
    else{
        return false;
    }
    return true;

}
