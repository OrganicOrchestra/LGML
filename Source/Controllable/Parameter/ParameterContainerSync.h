/*
  ==============================================================================

    ParameterContainerSync.h
    Created: 12 Oct 2017 10:02:38am
    Author:  Martin Hermant

  ==============================================================================
*/

#pragma once

#include "ParameterContainer.h"


class ParameterContainerSync:
public ControllableContainerListener

{
public:
    ParameterContainerSync(const String & name,ParameterContainer * root,ParameterContainer * slave);
    virtual ~ParameterContainerSync();
    void setRoot(ParameterContainer * root);
    
    virtual void clear() ;

    virtual ParameterContainer * createContainerFromContainer(ParameterContainer *p) = 0;
    ParameterContainer * getSlaveContainer();
    ParameterContainer * getSlaveRelatedContainer(ParameterContainer *c);
    ParameterContainer * getRootRelatedContainer(ParameterContainer *c);
    void checkContInSync(ParameterContainer * fromRoot);
    void checkContExists(ParameterContainer * fromRoot);

private:
    
    void controllableContainerAdded (ControllableContainer*, ControllableContainer*) override;
    void controllableContainerRemoved (ControllableContainer*, ControllableContainer*) override;
    void controllableFeedbackUpdate (ControllableContainer*, Controllable*) override;
    void childStructureChanged (ControllableContainer* /*notifier*/, ControllableContainer* /*origin*/,bool isAdded) override;
    void childAddressChanged (ControllableContainer* /*notifier*/, ControllableContainer* ) override;
    void controllableContainerPresetLoaded (ControllableContainer*) override;
    void containerCleared (ControllableContainer* /*origin*/) override;
    ParameterContainer *root;
    ParameterContainer * slave;
};
