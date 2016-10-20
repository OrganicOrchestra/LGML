/*
  ==============================================================================

    InspectorEditor.cpp
    Created: 9 May 2016 7:08:50pm
    Author:  bkupe

  ==============================================================================
*/

#include "InspectorEditor.h"

InspectorEditor::InspectorEditor(InspectableComponent * _sourceComponent) :
	sourceComponent(_sourceComponent)
{
	if (sourceComponent->relatedControllableContainer != nullptr)
	{
		DBG("Add FastMapperUI in inspectorEditor");
		fastMapperUI = new FastMapperUI(FastMapper::getInstance());
		fastMapperUI->setViewFilter(sourceComponent->relatedControllableContainer);
		addAndMakeVisible(fastMapperUI);
	}
}

InspectorEditor::~InspectorEditor()
{
}


void InspectorEditor::resized()
{
	Rectangle<int> r = getLocalBounds().removeFromBottom(fastMapperUI->getContentHeight());
	DBG("Set fast mapper ui size " << r.toString());
	fastMapperUI->setBounds(r);
	fastMapperUI->toFront(false);
}

int InspectorEditor::getContentHeight()
{
	return fastMapperUI->getContentHeight()+10; //will default to parent's bounds
}

void InspectorEditor::clear()
{
}
