/*
  ==============================================================================

    ControllableInspector.h
    Created: 27 Mar 2016 11:58:52am
    Author:  Martin Hermant

  ==============================================================================
*/

#ifndef CONTROLLABLEINSPECTOR_H_INCLUDED
#define CONTROLLABLEINSPECTOR_H_INCLUDED


#include "JuceHeader.h"
#include "ControllableContainerEditor.h"
#include "ControllableContainerProxy.h"


// side bar component displaying editor of currently selected nodes
// TODO handle merging of multiple component of sameClass

class ControllableInspector:public SelectableComponentHandler::SelectableHandlerListener,public Component{
public:
    ControllableInspector(NodeManagerUI * _nmui);
    virtual ~ControllableInspector(){}

void selectableChanged(SelectableComponent*  c,bool isSelected) override;
private:

    void addOrMergeControllableContainerEditor(ControllableContainer * c);
    void removeControllableContainerEditor(ControllableContainer * c);
    void generateFromCandidates();
   static void addControllableListenerToEditor(ControllableContainerEditor * source,ControllableContainer * listener);
    static bool hasSameControllableType(ControllableContainer * source,ControllableContainer * target);
    // create Controllables binded to UI that dispatch to multiple Controllables
    // each inspector UI element is binded to one Controllable in proxy,
    // if changed , it notifies All corresponding candidateControllable
    ScopedPointer<ControllableContainerProxy> proxyContainer;

    void generateProxyFromContainer(ControllableContainer * source);
    ScopedPointer<ControllableContainerEditor> displayedEditor;
    Array<ControllableContainer *> candidateContainers;







};


#endif  // CONTROLLABLEINSPECTOR_H_INCLUDED
