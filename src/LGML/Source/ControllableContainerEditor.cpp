/*
  ==============================================================================

    ControllableEditor.cpp
    Created: 26 Mar 2016 10:13:46pm
    Author:  Martin Hermant

  ==============================================================================
*/

#include "ControllableContainerEditor.h"


ControllableContainerEditor::ControllableContainerEditor(ControllableContainer * cc,Component* _embeddedComp):owner(cc),embeddedComp(_embeddedComp){

    if(embeddedComp){
        addAndMakeVisible(embeddedComp);
        setBounds(embeddedComp->getBounds());
    }
}


void ControllableContainerEditor::addControlUI(ControllableUI * c){
    controllableUIs.add(c);
    addAndMakeVisible(c);
}
void ControllableContainerEditor::removeControlUI(ControllableUI * c){
    removeChildComponent(c);
    controllableUIs.removeFirstMatchingValue(c);

}

void ControllableContainerEditor::removeControllableFromEditor(Controllable * toRemove){
    Array<ControllableUI *> arrayToRemove;
    for(auto & c:controllableUIs){
        if(c->controllable==toRemove){
            arrayToRemove.add(c);
        }
    }
    for(auto &r:arrayToRemove){
        controllableUIs.removeAllInstancesOf(r);
        delete r;
    }


    for(int i = 0 ; i < getNumChildComponents() ; i++){
        if(ControllableContainerEditor* c = dynamic_cast<ControllableContainerEditor*>(getChildComponent(i))){
            c->removeControllableFromEditor(toRemove);
        }
    }
}

void ControllableContainerEditor::removeContainerFromEditor(ControllableContainer * toRemove){
    Array<Component *> arrayToRemove;
    for(int i = 0 ; i < getNumChildComponents() ; i++){
        if(ControllableContainerEditor* c = dynamic_cast<ControllableContainerEditor*>(getChildComponent(i))){
            if(c->owner==toRemove){arrayToRemove.add(c);}
            else{c->removeContainerFromEditor(toRemove);}
        }
    }
    for(auto &r:arrayToRemove){
        removeChildComponent(r);
        delete r;
    }
}

void ControllableContainerEditor::paint(Graphics & g)
{
    g.fillAll(PANEL_COLOR);
	g.setColour(CONTOUR_COLOR);
	g.drawRect(getLocalBounds());
}

void ControllableContainerEditor::resized(){
    if(embeddedComp){
        embeddedComp->setBounds(getBounds());
    }
    for(auto &c:controllableUIs){
        c->setSize(getWidth(), c->getHeight());
    }
}
