/*
 ==============================================================================

 ControllableInspector.cpp
 Created: 27 Mar 2016 11:58:52am
 Author:  Martin Hermant

 ==============================================================================
 */

#include "ControllableInspector.h"


ControllableInspector::ControllableInspector(NodeManagerUI * nmui){
    nmui->addSelectedNodesListener(this);

}

void ControllableInspector::selectedNodeEvent(NodeBaseUI * node,bool state){

    if(node){
        if(state)
            addOrMergeControllableContainerEditor(node->node);
        else
            removeControllableContainerEditor(node->node);
    }

}
void ControllableInspector::addOrMergeControllableContainerEditor(ControllableContainer * c){
    candidateContainers.add(c);
    if(candidateContainers.size()==1){
        proxyContainer = new ControllableContainerProxy(c);
    }
    generateFromCandidates();
}


void ControllableInspector::generateFromCandidates(){
    removeAllChildren();


    if(candidateContainers.size()==0){return;}
    else if(candidateContainers.size()==1){
        displayedEditor = proxyContainer->sourceContainer->createControllableContainerEditor();
    }

    // try to merge common properties based on first
    else {
        if(displayedEditor==nullptr){
            // imossible that two candidate are added in one call
            jassertfalse;
            displayedEditor = proxyContainer->sourceContainer->createControllableContainerEditor();
        }
        for(auto &candidate:candidateContainers){
            proxyContainer->addProxyListener(candidate);


        }

    }

    jassert(displayedEditor!=nullptr);
    addAndMakeVisible(displayedEditor);
    // TODO default component are not resized...
    displayedEditor->setSize(getWidth(), displayedEditor->getHeight());
}

bool ControllableInspector::hasSameControllableType(ControllableContainer * source,ControllableContainer * target){
    return source == target;
}




void ControllableInspector::addControllableListenerToEditor(ControllableContainerEditor * source,ControllableContainer * listener){
    for(auto &c:source->controllableUIs){
        Controllable * cListener  =listener->getControllableByName(c->controllable->niceName);
        if(cListener){
//            c->addL
        }
        else{
            // should always find
            jassertfalse;
        }
    }


}
void ControllableInspector::removeControllableContainerEditor(ControllableContainer * c){
    candidateContainers.removeFirstMatchingValue(c);
    proxyContainer->removeProxyListener(c);
    if(candidateContainers.size()==0){
        delete proxyContainer.release();
    }
    generateFromCandidates();

}
