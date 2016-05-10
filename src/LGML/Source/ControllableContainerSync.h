/*
 ==============================================================================

 ControllableContainerSync.h
 Created: 27 Mar 2016 3:25:47pm
 Author:  Martin Hermant

 ==============================================================================
 */




#ifndef ControllableContainerSync_H_INCLUDED
#define ControllableContainerSync_H_INCLUDED

#define DEBUG_CONTROLLABLENAMES 0

#include "ControllableContainer.h"



/**
\brief a ControllableContainerSync can bi-directionnaly syncronyze a list of ControllableContainer

 other containers have to be of the same kind
 correspondences of Controllables is based on ControlAddress relative to ControllableContainers depth (i.e starting from the name of it)
 */


class ControllableContainerSync :public ControllableContainer, public ControllableContainer::Listener{

public:
    ControllableContainerSync(ControllableContainer * source,String overrideName);
    virtual ~ControllableContainerSync();


#if DEBUG_CONTROLLABLENAMES
    String groupName;
#endif
    void clear();

    void addSyncedControllableIfNotAlreadyThere(ControllableContainer * );///< tries to add a synced ControllableContainer
    void removeSyncedControllable(ControllableContainer * );///< tries to remove a synced ControllableContainer


    ControllableContainer * sourceContainer;

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




    Array<ControllableContainer*> targetSyncedContainers;
    void deepCopyForContainer(ControllableContainer * container);
    void doAddControllable(Controllable *c);
    void doRemoveControllable(Controllable * c);
    void doAddContainer(ControllableContainer * );
    void doRemoveContainer(ControllableContainer * );



    void controllableAdded(Controllable *c) override;
    void controllableRemoved(Controllable *c)override;
    void controllableContainerAdded(ControllableContainer *  c) override;
    void controllableContainerRemoved(ControllableContainer * c)override;
    void controllableFeedbackUpdate(Controllable *c) override;


    void onContainerParameterChanged(Parameter*)override;
    void onContainerTriggerTriggered(Trigger*)override;




    bool areCompatible(Controllable * target,Controllable * local){return produceGroupName(target->niceName) == local->niceName;}
    bool areCompatible(ControllableContainer * target,ControllableContainer * local){return produceGroupName(target->niceName) == local->niceName;}


    String produceGroupName(const String & n);


};
#endif  // ControllableContainerSync_H_INCLUDED

