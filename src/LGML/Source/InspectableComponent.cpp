/*
  ==============================================================================

    InspectableComponent.cpp
    Created: 9 May 2016 6:51:16pm
    Author:  bkupe

  ==============================================================================
*/

#include "InspectableComponent.h"
#include "Inspector.h"

#include "GenericControllableContainerEditor.h"
//#include "MainComponent.h"
#include "ShapeShifterFactory.h"
#include "ShapeShifterManager.h"

InspectableComponent::InspectableComponent(ControllableContainer * relatedContainer, const String &_inspectableType) :
	inspectableType(_inspectableType),
	relatedControllableContainer(relatedContainer),
	recursiveInspectionLevel(0),
	canInspectChildContainersBeyondRecursion(true),
	isSelected(false),
	paintBordersWhenSelected(true),
	bringToFrontOnSelect(true)
{
}

InspectableComponent::~InspectableComponent()
{
	listeners.call(&InspectableListener::inspectableRemoved,this);
}

InspectorEditor * InspectableComponent::createEditor()
{
	return new GenericControllableContainerEditor(relatedControllableContainer);
}

void InspectableComponent::selectThis()
{
	if (Inspector::getInstanceWithoutCreating() == nullptr)
	{
		ShapeShifterManager::getInstance()->showPanelWindowForContent(PanelName::InspectorPanel);
	}
	Inspector::getInstance()->setCurrentComponent(this);
}

void InspectableComponent::setSelected(bool value)
{
	if (value == isSelected) return;
	isSelected = value;

	if (bringToFrontOnSelect) toFront(true);
	 repaint();


	setSelectedInternal(value);

	listeners.call(&InspectableListener::inspectableSelectionChanged, this);
}

void InspectableComponent::setSelectedInternal(bool)
{
	//to be overriden
}

void InspectableComponent::paintOverChildren(juce::Graphics &g){
  if(isSelected && paintBordersWhenSelected){
    g.setColour( HIGHLIGHT_COLOR);
    g.drawRoundedRectangle(getLocalBounds().toFloat(), 4, 2);
  }

}
