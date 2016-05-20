/*
  ==============================================================================

    NodeContainerUI.cpp
    Created: 18 May 2016 7:54:08pm
    Author:  bkupe

  ==============================================================================
*/

#include "NodeContainerUI.h"
#include "NodeContainerContentUI.h"

NodeContainerUI::NodeContainerUI(NodeContainer * nc) :
	ConnectableNodeUI(nc,new NodeContainerContentUI()),
	nodeContainer(nc)
{
}

NodeContainerUI::~NodeContainerUI()
{
	clear();
}

void NodeContainerUI::clear()
{
}

void NodeContainerUI::nodeAdded(ConnectableNode *)
{
}

void NodeContainerUI::nodeRemoved(ConnectableNode *)
{
}

void NodeContainerUI::connectionAdded(NodeConnection *)
{
}

void NodeContainerUI::connectionRemoved(NodeConnection *)
{
}
