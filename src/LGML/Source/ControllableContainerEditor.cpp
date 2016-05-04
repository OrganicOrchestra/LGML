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


void ControllableContainerEditor::childBoundsChanged(Component * c)
{
	/*
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
	*/
}




void ControllableContainerEditor::childrenChanged(){

	/*
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
	*/

	resized();
};


void ControllableContainerEditor::paint(Graphics & g)
{
}

void ControllableContainerEditor::resized(){
	Rectangle<int> r = getLocalBounds().reduced(5,0);

    if(embeddedComp){
        embeddedComp->setBounds(r);
    }

	int gap = 5;
    for(int i = 0 ;i < getNumChildComponents() ; i++){
		Component * c = getChildComponent(i);
		c->setBounds(r.removeFromTop(20));
		r.removeFromTop(gap);
    }
}
