/*
  ==============================================================================

    NodeManager.cpp
    Created: 2 Mar 2016 8:33:39pm
    Author:  bkupe

  ==============================================================================
*/

#include "NodeManager.h"
#include "DummyNode.h"


NodeManager::NodeManager()
{

}

NodeManager::~NodeManager()
{
	clear();
}

void NodeManager::clear()
{
	DBG("Clear NodeManager");
	nodes.clear();
	audioGraph.clear();
	dataGraph.clear();
}

NodeBase * NodeManager::getNodeForId(const uint32 nodeId) const
{
	for (int i = nodes.size(); --i >= 0;)
		if (nodes.getUnchecked(i)->nodeId == nodeId)
			return nodes.getUnchecked(i);
}

NodeBase * NodeManager::addNode(NodeFactory::NodeType nodeType, uint32 nodeId)
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


	NodeBase * n = nodeFactory.createNode(nodeType, nodeId);
	nodes.add(n);
	n->addListener(this);
	listeners.call(&NodeManager::Listener::nodeAdded,n);

	//triggerAsyncUpdate();
	//n->setManager(this);

	return n;
}

bool NodeManager::removeNode(uint32 nodeId)
{
	DBG("Remove node from Node Manager, dispatch nodeRemoved to UI");
	NodeBase * n = getNodeForId(nodeId);
	if (n == nullptr) return false;
	n->removeListener(this);
	nodes.removeObject(n);
	listeners.call(&NodeManager::Listener::nodeRemoved, n);

	return true;
}



void NodeManager::addListener(Listener * const newListener)
{
	listeners.add(newListener);
}

void NodeManager::removeListener(Listener * const listener)
{
	listeners.remove(listener);
}

void NodeManager::askForRemoveNode(NodeBase * node)
{
	removeNode(node->nodeId);
}


