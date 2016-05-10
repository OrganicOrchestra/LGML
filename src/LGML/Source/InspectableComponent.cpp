/*
  ==============================================================================

    InspectableComponent.cpp
    Created: 9 May 2016 6:51:16pm
    Author:  bkupe

  ==============================================================================
*/

#include "InspectableComponent.h"
#include "Inspector.h"
#include "CustomEditor.h"
#include "GenericControllableContainerEditor.h"
#include "MainComponent.h"

InspectableComponent::InspectableComponent() : InspectableComponent(nullptr, &MainContentComponent::inspector)
{
}

InspectableComponent::InspectableComponent(ControllableContainer * relatedContainer) : InspectableComponent(relatedContainer, &MainContentComponent::inspector)
{
}

InspectableComponent::InspectableComponent(Inspector * targetInspector) : InspectableComponent(nullptr, targetInspector)
{
}

InspectableComponent::InspectableComponent(ControllableContainer * relatedContainer, Inspector * targetInspector) :
	inspector(targetInspector),
	relatedControllableContainer(relatedContainer),
	recursiveInspectionLevel(0),
	isSelected(false),
	repaintOnSelectionChanged(true)
{ 

}

InspectableComponent::~InspectableComponent()
{
}

InspectorEditor * InspectableComponent::getEditor()
{
	return new GenericControllableContainerEditor(this);
}

void InspectableComponent::selectThis()
{
	inspector->setCurrentComponent(this);
}

void InspectableComponent::setSelected(bool value)
{
	if (value == isSelected) return;
	isSelected = value;

	if (repaintOnSelectionChanged) repaint();
	 
	setSelectedInternal(value);
	
	listeners.call(&InspectableListener::inspectableSelectionChanged, this);
}

void InspectableComponent::setSelectedInternal(bool)
{
	//to be overriden
}
