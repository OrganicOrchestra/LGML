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

	}
	setSize(0, 0);
	resized();
	nodeManagerUIListeners.call(&NodeManagerUIListener::currentViewedContainerChanged);
  grabKeyboardFocus();
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
