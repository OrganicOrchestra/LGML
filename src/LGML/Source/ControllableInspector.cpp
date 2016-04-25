/*
 ==============================================================================

 ControllableInspector.cpp
 Created: 27 Mar 2016 11:58:52am
 Author:  Martin Hermant

 ==============================================================================
 */

#include "ControllableInspector.h"
#include "Style.h"

ControllableInspector::ControllableInspector(NodeManagerUI * _nmui):
controllableContainerSync(nullptr),
nmui(_nmui)
{
    nmui->selectableHandler.addSelectableHandlerListener(this);
}

ControllableInspector::~ControllableInspector(){
    nmui->selectableHandler.removeSelectableHandlerListener(this);
}

void ControllableInspector::selectableChanged(SelectableComponent * _node,bool state){
    NodeBaseUI * node = (NodeBaseUI*)_node;
    if(node){
        if(state)
            addOrMergeControllableContainerEditor(node->node);
        else
            removeControllableContainerEditor(node->node);
    }

}
void ControllableInspector::addOrMergeControllableContainerEditor(ControllableContainer * c){
    if(!candidateContainers.contains (c)){
        candidateContainers.add(c);
        c->addControllableContainerListener(this);
    }
    if(candidateContainers.size()==1){
        if(controllableContainerSync==nullptr){
            controllableContainerSync = new ControllableContainerSync(c);
        }
        else jassertfalse;
    }
    generateFromCandidates();
}


void ControllableInspector::generateFromCandidates(){
    removeAllChildren();


    if(candidateContainers.size()==0){return;}

    if(displayedEditor==nullptr ){
        displayedEditor =
        new ControllableContainerEditor(controllableContainerSync->sourceContainer,controllableContainerSync->sourceContainer->createControllableContainerEditor());
    }

    // regenerate a new one
    else if(displayedEditor->owner!=nullptr && displayedEditor->owner==controllableContainerSync->sourceContainer){
        delete displayedEditor.release();
        displayedEditor = new ControllableContainerEditor(controllableContainerSync->sourceContainer,controllableContainerSync->sourceContainer->createControllableContainerEditor());
    }



    // TODO :   -avoid recreating everything
    //          -try to merge common properties based on first by deleting non common params within candidateContainers
    for(auto &c:removedContainers){
        displayedEditor->removeContainerFromEditor(c);
    }

    for( auto &c:removedControllables){
        displayedEditor->removeControllableFromEditor(c);
    }

    for(auto &candidate:candidateContainers){controllableContainerSync->addSyncedControllableIfNotAlreadyThere(candidate);}

    addAndMakeVisible(displayedEditor);
    displayedEditor->setSize(getWidth(), displayedEditor->getHeight());
    setBounds(displayedEditor->getBounds().withPosition(0,0));


}







void ControllableInspector::removeControllableContainerEditor(ControllableContainer * c){

    candidateContainers.removeFirstMatchingValue(c);
    c->removeControllableContainerListener(this);
    removedControllables.clear();
    if(controllableContainerSync!=nullptr)controllableContainerSync->removeSyncedControllable(c);
    if(candidateContainers.size()==0){
        delete controllableContainerSync.release();
        controllableContainerSync=nullptr;
        delete displayedEditor.release();
        displayedEditor = nullptr;
    }
    generateFromCandidates();

}

void ControllableInspector::paint(Graphics &)
{
    //g.fillAll(PANEL_COLOR);
	
}

void ControllableInspector::resized(){
    if(displayedEditor)displayedEditor->setSize(getWidth(), displayedEditor->getHeight());
    repaint();
}
