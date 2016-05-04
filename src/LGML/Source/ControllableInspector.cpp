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
ShapeShifterContent("Inspector"),
controllableContainerSync(nullptr),
nmui(_nmui)
{
    nmui->selectableHandler.addSelectableHandlerListener(this);
}

ControllableInspector::~ControllableInspector(){
    nmui->selectableHandler.removeSelectableHandlerListener(this);
    if(controllableContainerSync){
        controllableContainerSync->removeContainerSyncListener(this);
        controllableContainerSync->removeControllableContainerListener(this);
    }
    controllableContainerSync = nullptr;
    displayedEditor = nullptr;
}

void ControllableInspector::selectableChanged(SelectableComponent * _node,bool state)
{
    NodeBaseUI * node = (NodeBaseUI*)_node;
    if(node){
        if(state)
            addOrMergeControllableContainerEditor(node->node);
        else
            removeControllableContainerEditor(node->node);
    }

}
void ControllableInspector::addOrMergeControllableContainerEditor(ControllableContainer * c)
{
    if(controllableContainerSync==nullptr){
        controllableContainerSync = new ControllableContainerSync(c,"editor");
        controllableContainerSync->addControllableContainerListener(this);
        controllableContainerSync->addContainerSyncListener(this);
    }
    controllableContainerSync->addSyncedControllableIfNotAlreadyThere(c);
    generateFromCandidates();
}


void ControllableInspector::removeControllableContainerEditor(ControllableContainer * c)
{
    if(controllableContainerSync && controllableContainerSync->sourceContainer == c){
        controllableContainerSync->removeControllableContainerListener(this);
        controllableContainerSync->removeContainerSyncListener(this);

        controllableContainerSync = nullptr;
        displayedEditor = nullptr;
        return;}
    if(controllableContainerSync!=nullptr)controllableContainerSync->removeSyncedControllable(c);

    generateFromCandidates();

}



void ControllableInspector::generateFromCandidates()
{
    if(controllableContainerSync==nullptr){return;}

    if(displayedEditor==nullptr )
	{
        displayedEditor = new ControllableContainerEditor(controllableContainerSync,nullptr);
        addAndMakeVisible(displayedEditor);
    }

    // regenerate a new one based on existing one
    else{
//        controllableContainerSync->createControllableContainerEditor(displayedEditor);
    }


    // TODO : -try to merge common properties based on first by deleting non common params within candidateContainers


	displayedEditor->setBounds(getLocalBounds());
    //setBounds(displayedEditor->getBounds());


}



void ControllableInspector::paint(Graphics &)
{
    //g.fillAll(PANEL_COLOR);

}

void ControllableInspector::resized()
{
    if(displayedEditor)displayedEditor->setSize(getWidth(), displayedEditor->getHeight());
    repaint();
}


void ControllableInspector::controllableAdded(Controllable *)
{
    generateFromCandidates();
};
void ControllableInspector::controllableRemoved(Controllable *)
{
    generateFromCandidates();
};
void ControllableInspector::controllableContainerAdded(ControllableContainer *)
{
    generateFromCandidates();
};

void ControllableInspector::controllableContainerRemoved(ControllableContainer * c)
{
    if(c==displayedEditor->owner){
        displayedEditor = nullptr;
    }
    generateFromCandidates();
}

void ControllableInspector::controllableFeedbackUpdate(Controllable *)
{
};


void ControllableInspector::sourceUpdated(ControllableContainer * c)
{

    displayedEditor = nullptr;
    controllableContainerSync = nullptr;
    addOrMergeControllableContainerEditor(c);

};

void ControllableInspector::structureChanged(){
    generateFromCandidates();
};
