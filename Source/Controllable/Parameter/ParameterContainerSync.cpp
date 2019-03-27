/*
 ==============================================================================
 
 ParameterContainerSync.cpp
 Created: 12 Oct 2017 10:02:38am
 Author:  Martin Hermant
 
 ==============================================================================
 */

#include "ParameterContainerSync.h"

static ShortNameType uidRefName("uid_ref"); // parameter keeping track of originating uid

ParameterContainerSync::ParameterContainerSync(const String& name,ParameterContainer *_slave=nullptr):root(nullptr),slave(_slave){
    if(!slave)slave = new ParameterContainer(name);
    slave->nameParam->setInternalOnlyFlags(true,false);
    
}
ParameterContainerSync::~ParameterContainerSync(){
    setRoot(nullptr);
}

void ParameterContainerSync::setRoot(ParameterContainer * _root){
    if(root){
        root->removeControllableContainerListener(this);
        clear();
    }
    root = _root;
    if(root){
        root->addControllableContainerListener(this);
        checkContInSync(root);
    }
    
}
ParameterContainer * ParameterContainerSync::getSlaveRelatedContainer(ParameterContainer *c,bool tryLastName){
    if(c==root){return slave;}

    ParameterContainer * inner = dynamic_cast<ParameterContainer*>(slave->getMirroredContainer(c,root));
    if( tryLastName && !inner){ // last resort try to find amongst uids
            const auto & ui = c->uid.toString();
            inner = dynamic_cast<ParameterContainer*>(slave->findFirstControllableContainer([ui](ControllableContainer*c){
                if(auto sui = dynamic_cast<StringParameter*>(c->getControllableByShortName(uidRefName))){
                    return sui->stringValue()== ui;
                }
                return false;
            }));
            if(!inner)
                jassertfalse;

    }
    
    
    return inner;
}

ParameterContainer * ParameterContainerSync::getRootRelatedContainer(ParameterContainer *c){
    return dynamic_cast<ParameterContainer*>(root->getMirroredContainer(c,slave));;
}

void ParameterContainerSync::clear () {
    //    slave->clearContainer();
};

ParameterContainer * ParameterContainerSync::getSlaveContainer(){
    return slave;
    
}





void ParameterContainerSync::controllableContainerAdded (ControllableContainer* /*notifier*/, ControllableContainer* cont) {
    checkContExists(dynamic_cast<ParameterContainer*>(cont));
    checkContInSync(dynamic_cast<ParameterContainer*>(cont));
}

void ParameterContainerSync::checkContExists(ParameterContainer * fromRoot){

    ScopedPointer<ParameterContainer> toCreate = createContainerFromContainer(fromRoot);
    if(!toCreate){
        return;
    }
    if(fromRoot!=root && !getSlaveRelatedContainer(fromRoot)){
        

        auto arr =fromRoot->getControlAddressRelative(root);
        

        ParameterContainer * slaveIt = slave;
        ParameterContainer * rootIt = root;

        // create object path if not exists
        for(auto& a:arr.getArray()){
            ParameterContainer * source =dynamic_cast<ParameterContainer*>(rootIt->getControllableContainerByShortName(a));
            ParameterContainer * target =dynamic_cast<ParameterContainer*>(slaveIt->getControllableContainerByShortName(a));
            if(!source){
                jassertfalse;
                return;
            }


            

            if( !target ){
                if( source==fromRoot){
                    target = toCreate.release();
                    target->setNiceName(fromRoot->getNiceName());
                }
                else{
                    target =createContainerFromContainer(source);
                    if(!target){
                        jassertfalse;
                        return;
//                        target = new ParameterContainer();
                    }
                    target->setNiceName(source->getNiceName());
                }

                target->nameParam->setInternalOnlyFlags(true,false);

                String ui = source->uid.toString();
                StringParameter * uidRefParam = dynamic_cast<StringParameter*>(target->getControllableByShortName(uidRefName));
                if(!uidRefParam){
                    uidRefParam =target->addNewUniqueParameter<StringParameter>(uidRefName.toString(), "uid reference", "none");
                    uidRefParam->setInternalOnlyFlags(true,false);
                }
                uidRefParam->setValue(ui);
                slaveIt->addChildControllableContainer(target);
            }

            
            

            

            
            
            slaveIt = target;
            rootIt = source;
        }
    }
}
void ParameterContainerSync::checkContInSync(ParameterContainer * fromRoot){
    if(!slave ){jassertfalse;return;}
    if(!fromRoot){jassertfalse;return;} // happens on deletions
    
    
    for(auto c:fromRoot->getContainersOfType<ParameterContainer>(false)){
        if(!getSlaveRelatedContainer(c)){
            controllableContainerAdded(nullptr, c);
        }
    }
    
    auto slaveCont = getSlaveRelatedContainer(fromRoot);
    if(slaveCont){
        for(auto c:slaveCont->getContainersOfType<ParameterContainer>(false)){
            if(! getRootRelatedContainer(c)){
                slaveCont->removeChildControllableContainer(c);
            }
        }
    }
};


void ParameterContainerSync::controllableContainerRemoved (ControllableContainer*, ControllableContainer* origin) {
    auto pc=dynamic_cast<ParameterContainer*>(origin);
    if(!pc){jassertfalse;return;}
    if(auto inner = getSlaveRelatedContainer(pc)){
        if(inner!=slave){
            inner->parentContainer->removeChildControllableContainer(inner);
        }
    }
    else{
        jassertfalse;
    }
    
};



void ParameterContainerSync::childStructureChanged (ControllableContainer* notifier, ControllableContainer* origin,bool /*isAdded*/) {
    
    if(notifier==root){
        auto pc=dynamic_cast<ParameterContainer*>(origin);
        if(!pc){jassertfalse;return;}
        checkContInSync(pc);
    }
    
    
    
    
};
void ParameterContainerSync::childAddressChanged (ControllableContainer* /*notifier*/,ControllableContainer* c) {
    if(auto inner = getSlaveRelatedContainer(dynamic_cast<ParameterContainer*>(c),true)){
        inner->setNiceName(c->getNiceName());
    }
    else{
        jassertfalse;// should find controller
    }
    
    
};

void ParameterContainerSync::containerWillClear (ControllableContainer* c) {
    auto pc=dynamic_cast<ParameterContainer*>(c);
    if(c==root){
        setRoot(nullptr);
        return;
    }
    if(!pc){
        jassertfalse; // hapen on closing file (destructor methods)
        return;
    }
    
    
    
    
    
};
