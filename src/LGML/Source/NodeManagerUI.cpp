/*
 ==============================================================================

 NodeManagerUI.cpp
 Created: 3 Mar 2016 10:38:22pm
 Author:  bkupe

 ==============================================================================
 */

#include "NodeManagerUI.h"
#include "NodeManager.h"

//==============================================================================
NodeManagerUI::NodeManagerUI(NodeManager * nodeManager) :
nodeManager(nodeManager),
currentViewer(nullptr)
{
	setCurrentViewedContainer(nodeManager->mainContainer);
}

NodeManagerUI::~NodeManagerUI()
{
	clear();
}

void NodeManagerUI::clear()
{
	setCurrentViewedContainer(nullptr);
}

void NodeManagerUI::resized()
{
	Rectangle<int> r = getLocalBounds();
	if (currentViewer != nullptr)
	{
		currentViewer->setBounds(r);
	}
}

int NodeManagerUI::getContentWidth()
{
	return (currentViewer == nullptr) ? 0 : currentViewer->getWidth();
}

int NodeManagerUI::getContentHeight()
{
	return (currentViewer == nullptr) ? 0 : currentViewer->getHeight();
}

void NodeManagerUI::setCurrentViewedContainer(NodeContainer * c)
{
	if (currentViewer != nullptr)
	{
		if (currentViewer->nodeContainer == c) return;

		removeChildComponent(currentViewer);
		currentViewer = nullptr;
	}

	if (c != nullptr)
	{
		currentViewer = new NodeContainerViewer(c);
		addAndMakeVisible(currentViewer);
	}
	resized();
}
void NodeManagerUI::childBoundsChanged(Component * )
{
	if (currentViewer != nullptr)
	{
		setSize(currentViewer->getWidth(), currentViewer->getHeight());
	}
}
