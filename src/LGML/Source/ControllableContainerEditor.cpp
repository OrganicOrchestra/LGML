/*
  ==============================================================================

    ControllableEditor.cpp
    Created: 26 Mar 2016 10:13:46pm
    Author:  Martin Hermant

  ==============================================================================
*/

#include "ControllableContainerEditor.h"


#include "ControllableUI.h"
#include "ControllableContainer.h"

ControllableContainerEditor::ControllableContainerEditor(ControllableContainer * cc,Component* _embeddedComp):
owner(cc),
embeddedComp(_embeddedComp)
{
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


void ControllableContainerEditor::childBoundsChanged(Component * c){
    int y = 0;
    int pad = 3;
    int maxW = 0;
    for(int i = 0 ; i < getNumChildComponents() ; i ++){
        Component * ch= getChildComponent(i);
        ch->setTopLeftPosition(0, y);
        y+=ch->getHeight()+pad;
        maxW = jmax(ch->getWidth(),maxW);

    }
    setSize(maxW,y);
}


void ControllableContainerEditor::syncUIElements(){
    if(owner==nullptr)return;

    for(auto & c:owner->controllables){
        bool found = false;
        for(auto & cc:controllableUIs){
            if( cc->controllable == c){
                found = true;
                break;
            }
        }
        if(!found){
            addControlUI(new NamedControllableUI(c->createDefaultControllableEditor(),100));
        }

    }


}


void ControllableContainerEditor::childrenChanged(){

    int y = 0;
    int pad = 3;
    int maxW = 0;
    for(int i = 0 ; i < getNumChildComponents() ; i ++){
        Component * ch= getChildComponent(i);
        ch->setTopLeftPosition(0, y);
        y+=ch->getHeight()+pad;
        maxW = jmax(ch->getWidth(),maxW);

    }
    setSize(maxW,y);
};
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
    for(int i = 0 ;i < getNumChildComponents() ; i++){
        getChildComponent(i)->setSize(getWidth(), getChildComponent(i)->getHeight());
    }
}
