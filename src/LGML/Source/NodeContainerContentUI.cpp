/*
  ==============================================================================

    NodeContainerContentUI.cpp
    Created: 20 May 2016 12:27:56pm
    Author:  bkupe

  ==============================================================================
*/

#include "NodeContainerContentUI.h"
#include "NodeManagerUI.h"
#include "ShapeShifterManager.h"

NodeContainerContentUI::NodeContainerContentUI() :
	ConnectableNodeContentUI(),
	editContainerBT("Edit Container")
{
	addAndMakeVisible(&editContainerBT);
	editContainerBT.addListener(this);
}

NodeContainerContentUI::~NodeContainerContentUI()
{
}

void NodeContainerContentUI::resized()
{
	editContainerBT.setBounds(getLocalBounds().withSizeKeepingCentre(100, 30));
}

void NodeContainerContentUI::init()
{
	nodeContainer = (NodeContainer *)node;
}

void NodeContainerContentUI::buttonClicked(Button * b)
{
	if (b == &editContainerBT)
	{
		NodeManagerUI * nmui = ((NodeManagerUIViewport *)ShapeShifterManager::getInstance()->getContentForName(PanelName::NodeManagerPanel))->nmui;
		if (nmui != nullptr)
		{
			nmui->setCurrentViewedContainer(nodeContainer);
		}
	}
}
