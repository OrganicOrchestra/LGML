/*
  ==============================================================================

    GenericControllableContainerEditor.cpp
    Created: 9 May 2016 6:41:59pm
    Author:  bkupe

  ==============================================================================
*/

#include "GenericControllableContainerEditor.h"
#include "InspectableComponent.h"
#include "ControllableUI.h"

GenericControllableContainerEditor::GenericControllableContainerEditor(InspectableComponent * sourceComponent) :
	InspectorEditor(sourceComponent)
{
	sourceContainer = sourceComponent->relatedControllableContainer;
	createUI();
}

GenericControllableContainerEditor::~GenericControllableContainerEditor()
{

}

void GenericControllableContainerEditor::createUI()
{
	if (sourceContainer == nullptr)
	{
		DBG("Source Container is null");
		return;
	}

	addUIForContainer(sourceContainer, sourceComponent->recursiveInspectionLevel);
}

void GenericControllableContainerEditor::addUIForContainer(ControllableContainer * cc, int recursiveLevel)
{
	for (auto &c : cc->controllables)
	{
		if (c->isControllableFeedbackOnly || !c->isControllableExposed) continue;

		ControllableUI * cui = c->createDefaultUI();
		controllablesUI.add(cui);
		addAndMakeVisible(cui);
	}

	if (recursiveLevel > 0)
	{
		for (auto &childCC : cc->controllableContainers)
		{
			addUIForContainer(childCC, recursiveLevel-1);
		}
	}
}

void GenericControllableContainerEditor::resized()
{
	Rectangle<int> r = getLocalBounds();

	for (auto &cui : controllablesUI)
	{
		cui->setBounds(r.removeFromTop(15));
		r.removeFromTop(2);
	}
}

void GenericControllableContainerEditor::clear()
{
	controllablesUI.clear();
}
