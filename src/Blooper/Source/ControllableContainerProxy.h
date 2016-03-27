/*
  ==============================================================================

    ControllableContainerProxy.h
    Created: 27 Mar 2016 3:25:47pm
    Author:  Martin Hermant

  ==============================================================================
*/

#ifndef CONTROLLABLECONTAINERPROXY_H_INCLUDED
#define CONTROLLABLECONTAINERPROXY_H_INCLUDED



// a proxyContainer can dispatch event from a source container to a list of container
// other containers have to be of the same kind

#include "ControllableContainer.h"
class ControllableContainerProxy : public ControllableContainer, ControllableContainer::Listener{

public:
    ControllableContainerProxy(ControllableContainer * source):
    ControllableContainer("proxy_"+source->niceName),
    sourceContainer(source),depthInOriginContainer(-1){
        buildFromContainer(source);
    }

    void buildFromContainer(ControllableContainer * source){
        source->addControllableContainerListener(this);
        depthInOriginContainer = 0;
        ControllableContainer * t = source;
        while(t!=nullptr){
            t = t->parentContainer;
            depthInOriginContainer++;
        }
    };


    void addProxyListener(ControllableContainer * );
    void removeProxyListener(ControllableContainer * );
    ControllableContainer * sourceContainer;

    Array<ControllableContainer*> controllableListeners;


    void controllableAdded(Controllable * c) override{};
    void controllableRemoved(Controllable * c)override{};
    void controllableContainerAdded(ControllableContainer * cc) override{};
    void controllableContainerRemoved(ControllableContainer * cc)override{};
    void controllableFeedbackUpdate(Controllable *c)override;
    int depthInOriginContainer;
};



#endif  // CONTROLLABLECONTAINERPROXY_H_INCLUDED
