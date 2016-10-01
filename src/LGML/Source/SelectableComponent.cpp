/*
  ==============================================================================

    SelectableComponent.cpp
    Created: 28 Mar 2016 3:13:08pm
    Author:  Martin Hermant

  ==============================================================================
*/

#include "SelectableComponent.h"
#include "SelectableComponentHandler.h"

SelectableComponent::SelectableComponent(SelectableComponentHandler * _handler, ControllableContainer * _relatedContainer):
	drawDefaultContourOnSelection(false),
	isSelected (false),
	repaintOnSelection(true),
	handler(_handler),
	selectableRelatedContainer(_relatedContainer)
{

}

SelectableComponent::~SelectableComponent(){
    handler->internalSelected(this,false,false);
}
void SelectableComponent::setHandler(SelectableComponentHandler * h){
    handler = h;
}

void SelectableComponent::askForSelection(bool _isSelected,bool unique ) {
    bool changed = _isSelected!=isSelected;
    isSelected = _isSelected;

    internalSetSelected(isSelected);

    if(handler!=nullptr && changed) handler->internalSelected(this,isSelected,unique);
    isSelected = _isSelected;

	if (changed)
	{
		selectableListeners.call(isSelected?&SelectableListener::componentSelected:&SelectableListener::componentDeselected,this);
	}

    if(repaintOnSelection && changed) repaint();
};

void SelectableComponent::paintOverChildren(juce::Graphics &g){

	if (!drawDefaultContourOnSelection) return;

    if(isSelected)
	{
        g.setColour(Colours::yellow);
        g.drawRect(getLocalBounds());
    }
}
