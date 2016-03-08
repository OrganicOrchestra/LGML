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
	connections.clear();
	audioGraph.clear();
	dataGraph.clear();
}

NodeBase * NodeManager::getNodeForId(const uint32 nodeId) const
{
	for (int i = nodes.size(); --i >= 0;)
		if (nodes.getUnchecked(i)->nodeId == nodeId)
			return nodes.getUnchecked(i);

	return nullptr;
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


	NodeBase * n = nodeFactory.createNode(this,nodeType, nodeId);
	nodes.add(n);
	n->addListener(this);
	listeners.call(&NodeManager::Listener::nodeAdded,n);

	return n;
}



bool NodeManager::removeNode(uint32 nodeId)
{
	DBG("Remove node from Node Manager, dispatch nodeRemoved to UI");
	NodeBase * n = getNodeForId(nodeId);
	
	Array<NodeConnection *> relatedConnections = getAllConnectionsForNode(n);

	for (auto &connection : relatedConnections) removeConnection(connection);

	if (n == nullptr) return false;
	n->removeListener(this);

	listeners.call(&NodeManager::Listener::nodeRemoved, n);
	nodes.removeObject(n);

	return true;
}



NodeConnection * NodeManager::getConnectionForId(const uint32 connectionId) const
{
	for (int i = connections.size(); --i >= 0;)
	{
		NodeConnection * c = connections.getUnchecked(i);
		if (c->connectionId == connectionId) return c;
	}

	return nullptr;
}

NodeConnection * NodeManager::getConnectionBetweenNodes(NodeBase * sourceNode, NodeBase * destNode, NodeConnection::ConnectionType connectionType)
{
	for (int i = connections.size(); --i >= 0;)
	{
		NodeConnection * c = connections.getUnchecked(i);
		if (c->sourceNode == sourceNode && c->destNode == destNode && c->connectionType == connectionType) return c;
	}

	return nullptr;
}

Array<NodeConnection*> NodeManager::getAllConnectionsForNode(NodeBase * node)
{
	Array<NodeConnection*> result;
	for (auto &connection : connections)
	{
		if (connection->sourceNode == node || connection->destNode == node) result.add(connection);
	}

	return result;
}

NodeConnection * NodeManager::addConnection(NodeBase * sourceNode, NodeBase * destNode, NodeConnection::ConnectionType connectionType, uint32 connectionId)
{
	DBG("Add Connection");
	if (getConnectionBetweenNodes(sourceNode, destNode, connectionType) != nullptr)
	{
		//connection already exists
		DBG("Connection already exists");
		return nullptr;
	}

	if (connectionId == 0)
	{
		connectionId = ++lastConnectionId;
	}
	else
	{
		// you can't add a node with an id that already exists in the graph..
		jassert(getConnectionForId(connectionId) == nullptr);
		removeConnection(connectionId);

		if (connectionId > lastConnectionId)
			lastConnectionId = connectionId;
	}


	NodeConnection * c = new NodeConnection(this,connectionId, sourceNode, destNode, connectionType);
	connections.add(c);
	c->addListener(this);

	DBG("Dispatch connection Added from NodeManager");
	listeners.call(&NodeManager::Listener::connectionAdded, c);

	return c;
}

bool NodeManager::removeConnection(uint32 connectionId)
{
	DBG("Remove connection from Node Manager, dispatch connectionRemoved to UI");
	NodeConnection * c = getConnectionForId(connectionId);
	return removeConnection(c);
}

bool NodeManager::removeConnection(NodeConnection * c)
{
	if (c == nullptr) return false;
	c->removeListener(this);

	connections.removeObject(c);

	listeners.call(&NodeManager::Listener::connectionRemoved, c);

	return true;
}



//From NodeBase Listener
void NodeManager::askForRemoveNode(NodeBase * node)
{
	removeNode(node->nodeId);
}

void NodeManager::connectionEdited(NodeConnection * connection)
{
	listeners.call(&NodeManager::Listener::connectionEdited, connection);
}

void NodeManager::askForRemoveConnection(NodeConnection *connection)
{
	removeConnection(connection->connectionId);
}




