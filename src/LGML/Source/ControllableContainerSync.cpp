/*
 ==============================================================================

 ControllableContainerSync.cpp
 Created: 27 Mar 2016 3:25:47pm
 Author:  Martin Hermant

 ==============================================================================
 */

#include "ControllableContainerSync.h"


// =============
// add athor container to be in sync

void ControllableContainerSync::addSyncedControllableIfNotAlreadyThere(ControllableContainer * c){
    // check that we have same class
    jassert(typeid(c)==typeid(sourceContainer));
    if(!targetSyncedContainers.contains(c)){
        c->addControllableContainerListener(this);
        targetSyncedContainers.add(c);

        for(auto & cc:controllableContainers){
            for(auto & ccc:c->controllableContainers){
                if(areCompatible(ccc,cc)){
                    ((ControllableContainerSync*)cc)->addSyncedControllableIfNotAlreadyThere(ccc);
                    break;
                }
            }
        }
    }
}
void ControllableContainerSync::removeSyncedControllable(ControllableContainer * c){
    if(c!=sourceContainer && !targetSyncedContainers.contains(c)){
        c->removeControllableContainerListener(this);
        targetSyncedContainers.removeAllInstancesOf(c);

        for(auto & cc:controllableContainers){
            for(auto & ccc:c->controllableContainers){
                if(areCompatible(ccc,cc)){
                    ((ControllableContainerSync*)cc)->removeSyncedControllable(ccc);
                    break;
                }
            }
        }
    }
}



// ====================
// internal structural methods

void ControllableContainerSync::deepCopyForContainer(ControllableContainer * container){

    for(auto & c:container->controllables){
        doAddControllable(c);
    }
    for(auto & c:container->controllableContainers){
        doAddContainer(c);
    }
    addSyncedControllableIfNotAlreadyThere(container);


}

void ControllableContainerSync::doAddControllable(Controllable *c){
    if(c->hideInEditor)return;
    if(Trigger * t = dynamic_cast<Trigger*>(c)){
        addTrigger(produceGroupName(t->niceName), t->description);
    }
    else if(FloatParameter * f = dynamic_cast<FloatParameter *>(c)){
        FloatParameter * ef = addFloatParameter(produceGroupName(f->niceName),f->description,f->defaultValue,f->minimumValue,f->maximumValue);
        ef->setValue(f->floatValue());
    }
    else if(IntParameter * f = dynamic_cast<IntParameter *>(c)){
        IntParameter * ef = addIntParameter(produceGroupName(f->niceName),f->description,f->defaultValue,f->minimumValue,f->maximumValue);
        ef->setValue(f->intValue());
    }
    else if(StringParameter * f = dynamic_cast<StringParameter *>(c)){
        StringParameter * ef = addStringParameter(produceGroupName(f->niceName),f->description,f->defaultValue);
        ef->setValue(f->stringValue());
    }
    else if(BoolParameter * f = dynamic_cast<BoolParameter *>(c)){
        BoolParameter * ef = addBoolParameter(produceGroupName(f->niceName),f->description,f->defaultValue);
        ef->setValue(f->boolValue());
    }
}

void ControllableContainerSync::doRemoveControllable(Controllable * c){

    for(auto & cc:controllables){
        if(areCompatible(c,cc)){
            removeControllable(cc);
            break;
        }
    }
}

void ControllableContainerSync::doAddContainer(ControllableContainer *c){
    ControllableContainerSync * cc = new ControllableContainerSync(c,groupName);
    addChildControllableContainer(cc);

}

void ControllableContainerSync::doRemoveContainer(ControllableContainer *c){
    for(auto & cc:controllableContainers){
        if(areCompatible(c,cc)){
            removeChildControllableContainer(cc);
            break;
        }
    }

    
}
void ControllableContainerSync::clear(){
    while(controllables.size()>0){
            removeControllable(controllables.getLast());
    }
    for(auto & c:controllableContainers){
        ((ControllableContainerSync*)c)->clear();
        delete c;
    }
    controllableContainers.clear();
}



// ===============================
// sync propagation


// from main
void ControllableContainerSync::onContainerParameterChanged(Parameter * c){
    for(auto & listener:targetSyncedContainers){
        for(auto t:listener->controllables){
            if(areCompatible(t ,c)){
                setControllableValue(c, t);
            }
        }
    }
}
void ControllableContainerSync::onContainerTriggerTriggered(Trigger * c){
    for(auto & listener:targetSyncedContainers){
        for(auto t:listener->controllables){
            if(areCompatible(t ,c)){
                setControllableValue(c, t);
            }
        }
    }
}


// from list
void ControllableContainerSync::controllableFeedbackUpdate(Controllable *c) {
    if(c->parentContainer ){
    if(areCompatible(c->parentContainer,this)){
        for(auto & cc:controllables){
            if(areCompatible(c, cc)){
                setControllableValue(c, cc);

            }
        }
    }
    }
    else{
        jassertfalse;

    }
};



bool ControllableContainerSync::setControllableValue(Controllable * cOrigin,Controllable * c){
    if(cOrigin==notifyingControllable)return false;
    jassert(cOrigin->type == c->type);


    if (c != nullptr )
    {

        notifyingControllable = cOrigin;
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
            notifyingControllable = nullptr;
    }
    else{
        return false;
    }
    return true;


}


// === sync listener{
void ControllableContainerSync::notifyStructureChanged(){
    containerSyncListeners.call(&ContainerSyncListener::structureChanged);
    ControllableContainerSync * parent = dynamic_cast<ControllableContainerSync*>(parentContainer);
    if(parent){
        parent->notifyStructureChanged();
    }
}

void ControllableContainerSync::controllableAdded(Controllable *c) {
    doAddControllable(c);
    notifyStructureChanged();

}

void ControllableContainerSync::controllableRemoved(Controllable *c){
    doRemoveControllable(c);
    notifyStructureChanged();
}
void ControllableContainerSync::controllableContainerAdded(ControllableContainer *  c) {
    doAddContainer(c);
    notifyStructureChanged();
}
void ControllableContainerSync::controllableContainerRemoved(ControllableContainer * c){
    doRemoveContainer(c);
    notifyStructureChanged();
}
