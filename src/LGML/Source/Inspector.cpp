/*
  ==============================================================================

    Inspector.cpp
    Created: 9 May 2016 6:41:38pm
    Author:  bkupe

  ==============================================================================
*/

#include "Inspector.h"

Inspector::Inspector() :
	ShapeShifterContent("Inspector"),
	currentComponent(nullptr)
{
}

Inspector::~Inspector()
{
	clear();
}

void Inspector::setCurrentComponent(InspectableComponent * c)
{
	if (c == currentComponent) return;

	DBG("Set current Component");

	if (currentComponent != nullptr)
	{
		clear();
		currentComponent->setSelected(false);
	}

	currentComponent = c;

	if (currentComponent != nullptr)
	{
		currentComponent->setSelected(true);
		inspectCurrentComponent();
	}
}

void Inspector::resized()
{
	if (currentEditor != nullptr) currentEditor->setBounds(getLocalBounds().reduced(5));
}

void Inspector::clear()
{
	if (currentEditor != nullptr)
	{
		removeChildComponent(currentEditor);
		currentEditor->clear();
		currentEditor = nullptr;
	}
}

void Inspector::inspectCurrentComponent()
{
	if (currentComponent == nullptr) return;
	currentEditor = currentComponent->getEditor();
	addAndMakeVisible(currentEditor);
	resized();
}
