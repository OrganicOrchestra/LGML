/*
  ==============================================================================

    Inspector.cpp
    Created: 9 May 2016 6:41:38pm
    Author:  bkupe

  ==============================================================================
*/

#include "Inspector.h"

Inspector::Inspector() :
	currentComponent(nullptr),
	isEnabled(true)
{
}

Inspector::~Inspector()
{
	clear();
}

void Inspector::setEnabled(bool value)
{
	if (isEnabled == value) return;
	
	if (!value) setCurrentComponent(nullptr);
	isEnabled = value;
}

void Inspector::setCurrentComponent(InspectableComponent * c)
{
	if (c == currentComponent) return;
	if (!isEnabled) return;

	if (currentComponent != nullptr)
	{
		clear();
		currentComponent->setSelected(false);
		currentComponent->removeInspectableListener(this);
	}

	currentComponent = c;

	if (currentComponent != nullptr)
	{
		currentComponent->setSelected(true);
		currentComponent->addInspectableListener(this);
		inspectCurrentComponent();
	}

	listeners.call(&InspectorListener::currentComponentChanged, this);
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

void Inspector::inspectableRemoved(InspectableComponent * component)
{
	if (component == currentComponent) setCurrentComponent(nullptr);
}
