/*
  ==============================================================================

    SelectableComponent.cpp
    Created: 28 Mar 2016 3:13:08pm
    Author:  Martin Hermant

  ==============================================================================
*/

#include "SelectableComponent.h"

#include "SelectableComponentHandler.h"


SelectableComponent::SelectableComponent(SelectableComponentHandler * _handler):isSelected (false),repaintOnSelection(true),handler(_handler){

}
void SelectableComponent::setHandler(SelectableComponentHandler * h){
    handler = h;
}

void SelectableComponent::askForSelection(bool _isSelected,bool unique ) {
    isSelected = _isSelected;
    internalSetSelected(isSelected);
    if(handler!=nullptr)handler->internalSelected(this,isSelected,unique);
    if(repaintOnSelection)repaint();
};
void SelectableComponent::paintOverChildren(juce::Graphics &g){
if(isSelected){
    g.setColour(Colours::yellow);
    g.drawRect(getLocalBounds());
}
}