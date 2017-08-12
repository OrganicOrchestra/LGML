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


#include "NodeManagerUI.h"
#include "NodeManager.h"

//==============================================================================
NodeManagerUI::NodeManagerUI(NodeManager * nodeManager) :
nodeManager(nodeManager),
currentViewer(nullptr)
{
	nodeManager->addNodeManagerListener(this);
	setCurrentViewedContainer(nodeManager->mainContainer);

}

NodeManagerUI::~NodeManagerUI()
{
	nodeManager->removeNodeManagerListener(this);
	clear();
	setCurrentViewedContainer(nullptr);

}

void NodeManagerUI::clear()
{
	setCurrentViewedContainer(nodeManager->mainContainer);
}

void NodeManagerUI::resized()
{

	Rectangle<int> r = getLocalBounds();
	if (currentViewer != nullptr)
	{
		currentViewer->setTopLeftPosition(0, 0);
		currentViewer->setSize(jmax<int>(getWidth(), currentViewer->getWidth()), jmax<int>(getHeight(), currentViewer->getHeight()));
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

void NodeManagerUI::managerCleared()
{
	clear();
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
		currentViewer->setTopLeftPosition(0, 0);
    currentViewer->setSelected(true);

	}
	setSize(0, 0);
	resized();
	nodeManagerUIListeners.call(&NodeManagerUIListener::currentViewedContainerChanged);

}
void NodeManagerUI::childBoundsChanged(Component * )
{
	if (currentViewer != nullptr)
	{
		setSize(currentViewer->getWidth(), currentViewer->getHeight());
	}
}

bool NodeManagerUI::keyPressed(const KeyPress & key){
  if(key.getModifiers().isCommandDown() && key.getKeyCode()==KeyPress::upKey){
    if(NodeContainer * c = (currentViewer->nodeContainer->parentNodeContainer)){
        setCurrentViewedContainer(c);
        return true;

    }
  }

    return false;
}
