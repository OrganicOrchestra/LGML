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


class ControllableContainerSync :public ControllableContainer, public ControllableContainer::Listener{

public:
    ControllableContainerSync(ControllableContainer * source,String _groupName):
    groupName(_groupName),
    ControllableContainer(source->niceName),
    sourceContainer(source),
    notifyingControllable(nullptr)
    {
        setNiceName(produceGroupName(source->niceName));
        deepCopyForContainer(source);
        
    }

    virtual ~ControllableContainerSync(){
        for(auto &c:targetSyncedContainers){
            c->removeControllableContainerListener(this);
        }
        if(sourceContainer)
            sourceContainer->removeControllableContainerListener(this);

        clear();
    }

    String groupName;

    bool areCompatible(Controllable * target,Controllable * local){return produceGroupName(target->niceName) == local->niceName;}
    bool areCompatible(ControllableContainer * target,ControllableContainer * local){return produceGroupName(target->niceName) == local->niceName;}
    String produceGroupName(const String & n){return groupName+"_"+n;}
    void deepCopyForContainer(ControllableContainer * container);
    void doAddControllable(Controllable *c);
    void doRemoveControllable(Controllable * c);
    void clear();

    void addSyncedControllableIfNotAlreadyThere(ControllableContainer * );
    void removeSyncedControllable(ControllableContainer * );
    ControllableContainer * sourceContainer;

    Array<ControllableContainer*> targetSyncedContainers;


    void controllableAdded(Controllable *c) override{doAddControllable(c);};
    void controllableRemoved(Controllable *c)override{doRemoveControllable(c);};
    void controllableContainerAdded(ControllableContainer *) override{};
    void controllableContainerRemoved(ControllableContainer *)override{};
    void controllableFeedbackUpdate(Controllable *c) override;

    
    void onContainerParameterChanged(Parameter*)override;
    void onContainerTriggerTriggered(Trigger*)override;


    class ContainerSyncListener{
    public:
        virtual ~ContainerSyncListener(){}

        virtual void sourceUpdated(ControllableContainer * )=0;
        virtual void sourceDeleted(){};

    };

    ListenerList<ContainerSyncListener> containerSyncListeners;
    void addContainerSyncListener(ContainerSyncListener * l){containerSyncListeners.add(l);}
    void removeContainerSyncListener(ContainerSyncListener * l){containerSyncListeners.remove(l);}



private:
    Controllable * notifyingControllable;

    bool setControllableValue(Controllable * cOrigin,Controllable * c);
};



#endif  // ControllableContainerSync_H_INCLUDED
