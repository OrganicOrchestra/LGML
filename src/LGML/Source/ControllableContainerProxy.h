/*
 ==============================================================================

 ControllableContainerProxy.h
 Created: 27 Mar 2016 3:25:47pm
 Author:  Martin Hermant

 ==============================================================================
 */

#ifndef CONTROLLABLECONTAINERPROXY_H_INCLUDED
#define CONTROLLABLECONTAINERPROXY_H_INCLUDED

// a proxyContainer can fully syncronyze Controllables from a source container to a list of container
// other containers have to be of the same kind
// correspondences of Controllables is based on ControlAddress relative to ControllableContainers depth (i.e starting from the name of it
// TODO handle controllableContainerAdded / Removed sync

#include "ControllableContainer.h"
class ControllableContainerProxy : public ControllableContainer, ControllableContainer::Listener{

public:
    ControllableContainerProxy(ControllableContainer * source):
    ControllableContainer("proxy_"+source->niceName),
    sourceContainer(source),depthInOriginContainer(-1),
    isNotifying(false){
        buildFromContainer(source);
    }

    virtual ~ControllableContainerProxy(){
        for(auto &c:proxyControllableListeners){
            c->removeControllableContainerListener(this);
        }
        if(sourceContainer)
            sourceContainer->removeControllableContainerListener(this);
    }

    void buildFromContainer(ControllableContainer * source);

    void addProxyListener(ControllableContainer * );
    void removeProxyListener(ControllableContainer * );
    ControllableContainer * sourceContainer;

    Array<ControllableContainer*> proxyControllableListeners;


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



#endif  // CONTROLLABLECONTAINERPROXY_H_INCLUDED
