/*
  ==============================================================================

    NodeContainerUI.cpp
    Created: 18 May 2016 7:54:08pm
    Author:  bkupe

  ==============================================================================
*/

#include "NodeContainerUI.h"

NodeContainerUI::NodeContainerUI(NodeContainer * nc) :
	ConnectableNodeUI(nc),
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

void NodeContainerUI::nodeAdded(NodeBase *)
{
}

void NodeContainerUI::nodeRemoved(NodeBase *)
{
}

void NodeContainerUI::connectionAdded(NodeConnection *)
{
}

void NodeContainerUI::connectionRemoved(NodeConnection *)
{
}
