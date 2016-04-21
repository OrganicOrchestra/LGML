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

void ControllableContainerEditor::paint(Graphics & g)
{
	g.fillAll(BG_COLOR);
}

void ControllableContainerEditor::resized(){
    if(embeddedComp){
        embeddedComp->setBounds(getBounds());
    }
    for(auto &c:controllableUIs){
        c->setSize(getWidth(), c->getHeight());
    }
}
