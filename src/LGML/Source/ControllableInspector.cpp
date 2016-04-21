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
    proxyContainer(nullptr),
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
    candidateContainers.addIfNotAlreadyThere(c);
    if(candidateContainers.size()==1){
        if(proxyContainer==nullptr)
            proxyContainer = new ControllableContainerProxy(c);
        else jassertfalse;
    }
    generateFromCandidates();
}


void ControllableInspector::generateFromCandidates(){
    removeAllChildren();


    if(candidateContainers.size()==0){return;}

    if(displayedEditor==nullptr )displayedEditor = new ControllableContainerEditor(
                                                                                   proxyContainer->sourceContainer,proxyContainer->sourceContainer->createControllableContainerEditor());
    else if(displayedEditor->owner!=nullptr && displayedEditor->owner==proxyContainer->sourceContainer){
        delete displayedEditor.release();
        displayedEditor = new ControllableContainerEditor(proxyContainer->sourceContainer,proxyContainer->sourceContainer->createControllableContainerEditor());}


    // try to merge common properties based on first

    for(auto &candidate:candidateContainers){
        proxyContainer->addProxyListener(candidate);
    }

    addAndMakeVisible(displayedEditor);

    displayedEditor->setSize(getWidth(), displayedEditor->getHeight());
    setBounds(displayedEditor->getBounds().withPosition(0,0));


}







void ControllableInspector::removeControllableContainerEditor(ControllableContainer * c){
    candidateContainers.removeFirstMatchingValue(c);

    if(proxyContainer!=nullptr)proxyContainer->removeProxyListener(c);
    if(candidateContainers.size()==0){
        delete proxyContainer.release();
        proxyContainer=nullptr;
        delete displayedEditor.release();
        displayedEditor = nullptr;
    }
    generateFromCandidates();

}

void ControllableInspector::paint(Graphics & g)
{
	g.fillAll(BG_COLOR);
}

void ControllableInspector::resized(){
if(displayedEditor)displayedEditor->setSize(getWidth(), displayedEditor->getHeight());
repaint();
}
