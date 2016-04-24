/*
 ==============================================================================

 ControllableContainerSync.h
 Created: 27 Mar 2016 3:25:47pm
 Author:  Martin Hermant

 ==============================================================================
 */

#ifndef ControllableContainerSync_H_INCLUDED
#define ControllableContainerSync_H_INCLUDED

// a ControllableContainerSync can fully syncronyze Controllables from a source container to a list of container
// other containers have to be of the same kind
// correspondences of Controllables is based on ControlAddress relative to ControllableContainers depth (i.e starting from the name of it
// TODO handle controllableContainerAdded / Removed sync

#include "ControllableContainer.h"
class ControllableContainerSync : public ControllableContainer::Listener{

public:
    ControllableContainerSync(ControllableContainer * source):
    sourceContainer(source),depthInOriginContainer(-1),
    isNotifying(false){
        buildFromContainer(source);
    }

    virtual ~ControllableContainerSync(){
        for(auto &c:targetSyncedContainers){
            c->removeControllableContainerListener(this);
        }
        if(sourceContainer)
            sourceContainer->removeControllableContainerListener(this);
    }

    void buildFromContainer(ControllableContainer * source);

    void addSyncedControllableIfNotAlreadyThere(ControllableContainer * );
    void removeSyncedControllable(ControllableContainer * );
    ControllableContainer * sourceContainer;

    Array<ControllableContainer*> targetSyncedContainers;


    void controllableAdded(Controllable *) override{};
    void controllableRemoved(Controllable *)override{};
    void controllableContainerAdded(ControllableContainer *) override{};
    void controllableContainerRemoved(ControllableContainer *)override{};
    void controllableFeedbackUpdate(Controllable *c) override;
    int depthInOriginContainer;




private:
    bool isNotifying;

    bool setControllableValue(Controllable * cOrigin,Controllable * c);
};



#endif  // ControllableContainerSync_H_INCLUDED
