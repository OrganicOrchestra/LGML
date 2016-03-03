/*
  ==============================================================================

    NodeManager.cpp
    Created: 2 Mar 2016 8:33:39pm
    Author:  bkupe

  ==============================================================================
*/

#include "NodeManager.h"


NodeManager::NodeManager()
{
	audioGraph = new AudioProcessorGraph();
	dataGraph = new DataProcessorGraph();
}

void NodeManager::clear()
{
	nodes.clear();
	audioGraph->clear();
	dataGraph->clear();
}

NodeBase * NodeManager::getNodeForId(const uint32 nodeId) const
{
	for (int i = nodes.size(); --i >= 0;)
		if (nodes.getUnchecked(i)->nodeId == nodeId)
			return nodes.getUnchecked(i);
}

NodeBase * NodeManager::addNode(uint32 nodeId)
{
	if (nodeId == 0)
	{
		nodeId = ++lastNodeId;
	}
	else
	{
		// you can't add a node with an id that already exists in the graph..
		jassert(getNodeForId(nodeId) == nullptr);
		removeNode(nodeId);

		if (nodeId > lastNodeId)
			lastNodeId = nodeId;
	}


	NodeBase* n = new NodeBase(nodeId);
	nodes.add(n);

	//triggerAsyncUpdate();
	//n->setManager(this);

	return n;
}

bool NodeManager::removeNode(uint32 nodeId)
{
	return false;
}
