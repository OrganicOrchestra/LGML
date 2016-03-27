/*
  ==============================================================================

    ControllableEditor.cpp
    Created: 26 Mar 2016 10:13:46pm
    Author:  Martin Hermant

  ==============================================================================
*/

#include "ControllableContainerEditor.h"


ControllableContainerEditor::ControllableContainerEditor(ControllableContainer * cc):owner(cc){


};


void ControllableContainerEditor::addControlUI(ControllableUI * c){
    controllableUIs.add(c);
    addAndMakeVisible(c);
}
void ControllableContainerEditor::removeControlUI(ControllableUI * c){
    controllableUIs.removeFirstMatchingValue(c);
    removeChildComponent(c);
}