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
	recursiveInspectionLevel = 1;
	canInspectChildContainersBeyondRecursion = true;

}

NodeContainerUI::~NodeContainerUI()
{
	clear();
}

void NodeContainerUI::clear()
{
}
