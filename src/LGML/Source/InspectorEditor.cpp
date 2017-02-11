/*
  ==============================================================================

    InspectorEditor.cpp
    Created: 9 May 2016 7:08:50pm
    Author:  bkupe

  ==============================================================================
*/

#include "InspectorEditor.h"

InspectorEditor::InspectorEditor():
fastMapperUI(nullptr)
{

  // TODO subclass this
//	if (sourceComponent->relatedControllableContainer != nullptr)
//	{
//		DBG("Add FastMapperUI in InspectorEditor");
//		fastMapperUI = new FastMapperUI(FastMapper::getInstance());
//		fastMapperUI->setViewFilter(sourceComponent->relatedControllableContainer);
//		addAndMakeVisible(fastMapperUI);
//	}
}

InspectorEditor::~InspectorEditor()
{
}


void InspectorEditor::resized()
{
	if (fastMapperUI != nullptr)
	{
		Rectangle<int> r = getLocalBounds().removeFromBottom(fastMapperUI->getContentHeight());
		DBG("Set fast mapper ui size " << r.toString());
		fastMapperUI->setBounds(r);
		fastMapperUI->toFront(false);
	}
}

int InspectorEditor::getContentHeight()
{
	if (fastMapperUI == nullptr) return 0;//will default to parent's bounds
	return fastMapperUI->getContentHeight()+10; 
}

void InspectorEditor::clear()
{
}
