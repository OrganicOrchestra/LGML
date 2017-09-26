/* Copyright © Organic Orchestra, 2017
*
* This file is part of LGML.  LGML is a software to manipulate sound in realtime
*
* This program is free software; you can redistribute it and/or modify
* it under the terms of the GNU General Public License as published by
* the Free Software Foundation (version 3 of the License).
*
* This program is distributed in the hope that it will be useful,
* but WITHOUT ANY WARRANTY; without even the implied warranty of
* MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
*
*/


#include "InspectableComponent.h"
#include "Inspector.h"

#include "../../Controllable/Parameter/UI/GenericParameterContainerEditor.h"
#include "../ShapeShifter/ShapeShifterFactory.h"
#include "../ShapeShifter/ShapeShifterManager.h"
#include "../Style.h"

InspectableComponent::InspectableComponent(ParameterContainer * relatedContainer, const String &_inspectableType) :
	inspectableType(_inspectableType),
	relatedParameterContainer(relatedContainer),
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
	return new GenericParameterContainerEditor(relatedParameterContainer);
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
    g.setColour( findColour(TextButton::buttonOnColourId));
    g.drawRoundedRectangle(getLocalBounds().toFloat(), 4, 2);
  }

}
