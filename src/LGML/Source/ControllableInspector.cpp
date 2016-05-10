/*
 ==============================================================================

 ControllableInspector.cpp
 Created: 27 Mar 2016 11:58:52am
 Author:  Martin Hermant

 ==============================================================================
 */

#include "ControllableInspector.h"
#include "Style.h"

ControllableInspector::ControllableInspector(SelectableComponentHandler * _handler):
ShapeShifterContent("Inspector"),
controllableContainerSync(nullptr),
handler(_handler)
{
    handler->addSelectableHandlerListener(this);
}

ControllableInspector::~ControllableInspector()
{
   handler->removeSelectableHandlerListener(this);
    if(controllableContainerSync){
        controllableContainerSync->removeContainerSyncListener(this);
        controllableContainerSync->removeControllableContainerListener(this);
    }
    controllableContainerSync = nullptr;
    displayedEditor = nullptr;
}

void ControllableInspector::selectableChanged(SelectableComponent * c ,bool state)
{
	ControllableContainer * cc = c->selectableRelatedContainer;
    if(state)
        addOrMergeControllableContainerEditor(cc);
    else
        removeControllableContainerEditor(cc);

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
    if(controllableContainerSync == nullptr){return;}

    if(displayedEditor==nullptr )
	{
        displayedEditor = new ControllableContainerEditor(controllableContainerSync,nullptr);
        addAndMakeVisible(displayedEditor);
    }

    // regenerate a new one based on existing one
    else{
//        controllableContainerSync->createDefaultUI(displayedEditor);
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
    if(displayedEditor) displayedEditor->setSize(getWidth(), displayedEditor->getHeight());
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

void ControllableInspector::childStructureChanged(ControllableContainer * ){
    generateFromCandidates();
};
