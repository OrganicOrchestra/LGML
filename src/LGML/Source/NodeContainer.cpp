/*
  ==============================================================================

    NodeContainer.cpp
    Created: 18 May 2016 7:53:56pm
    Author:  bkupe

  ==============================================================================
*/

#include "NodeContainer.h"
#include "NodeManager.h"
#include "NodeConnection.h"
#include "NodeContainerUI.h"

NodeContainer::NodeContainer(const String &name, NodeContainer * _parentNodeContainer) :
	parentNodeContainer(_parentNodeContainer),
	ConnectableNode(name,NodeType::ContainerType)
{
	saveAndLoadRecursiveData = false;

	//if (parentNodeContainer != nullptr)
	//{
		containerInNode = (ContainerInNode *)addNode(new ContainerInNode());
		containerOutNode = (ContainerOutNode *)addNode(new ContainerOutNode());
	//}
}


NodeContainer::~NodeContainer()
{
	clear(false);
}

void NodeContainer::clear(bool recreateContainerNodes)
{

	while (nodes.size() > 0)
	{
		nodes[0]->remove();
	}

	connections.clear();

	containerInNode = nullptr;
	containerOutNode = nullptr;

	if (recreateContainerNodes)
	{
		containerInNode = (ContainerInNode *)addNode(new ContainerInNode());
		containerOutNode = (ContainerOutNode *)addNode(new ContainerOutNode());
	}
}


ConnectableNode * NodeContainer::addNode(NodeType nodeType)
{
	ConnectableNode * n = NodeFactory::createNode(nodeType);
	return addNode(n);
}

ConnectableNode * NodeContainer::addNode(ConnectableNode * n)
{
	nodes.add(n);
	n->addNodeListener(this);
	n->nameParam->setValue(getUniqueNameInContainer(n->nameParam->stringValue()));
	addChildControllableContainer(n); //ControllableContainer
	nodeContainerListeners.call(&NodeContainerListener::nodeAdded, n);
	return n;
}



bool NodeContainer::removeNode(ConnectableNode * n)
{
	Array<NodeConnection *> relatedConnections = getAllConnectionsForNode(n);

	for (auto &connection : relatedConnections) removeConnection(connection);

	if (n == nullptr) return false;
	n->removeNodeListener(this);
	removeChildControllableContainer(n);

	nodeContainerListeners.call(&NodeContainerListener::nodeRemoved, n);
	nodes.removeAllInstancesOf(n);

	n->removeFromAudioGraph();

	//if(NodeManager::getInstanceWithoutCreating() != nullptr) NodeManager::getInstance()->audioGraph.removeNode(n->audioNode);

	return true;
}

ConnectableNode * NodeContainer::getNodeForName(const String & name)
{
	for (auto &n : nodes)
	{
		if (n->shortName == name) return n;
	}
	return nullptr;
}



int NodeContainer::getNumConnections() {
	return connections.size();
}

var NodeContainer::getJSONData()
{
	var data = ControllableContainer::getJSONData();
	var nodesData;

	for (auto &n : nodes)
	{
		nodesData.append(n->getJSONData());
	}

	var connectionsData;

	for (auto &c : connections)
	{
		connectionsData.append(c->getJSONData());
	}

	data.getDynamicObject()->setProperty("nodes", nodesData);
	data.getDynamicObject()->setProperty("connections", connectionsData);
	return data;
}

void NodeContainer::loadJSONDataInternal(var data)
{
	clear(true);

	Array<var> * nodesData = data.getProperty("nodes", var()).getArray();
	for (var &nData : *nodesData)
	{
		NodeType nodeType = NodeFactory::getTypeFromString(nData.getProperty("nodeType", var()));
		ConnectableNode * node = addNode(nodeType);
		node->loadJSONData(nData);
	}

	Array<var> * connectionsData = data.getProperty("connections", var()).getArray();

	if (connectionsData)
	{
		for (var &cData : *connectionsData)
		{
			ConnectableNode * srcNode = getNodeForName(cData.getDynamicObject()->getProperty("srcNode").toString());
			ConnectableNode * dstNode = getNodeForName(cData.getDynamicObject()->getProperty("dstNode").toString());

			int cType = cData.getProperty("connectionType", var());

			if (srcNode && dstNode && isPositiveAndBelow(cType, (int)NodeConnection::ConnectionType::UNDEFINED)) {
				NodeConnection * c = addConnection(srcNode, dstNode, NodeConnection::ConnectionType(cType));
				c->loadJSONData(cData);
			}
			else {
				// TODO nicely handle file format errors?
				jassertfalse;
			}
		}
	}

	removeIllegalConnections();

}


NodeConnection * NodeContainer::getConnectionBetweenNodes(ConnectableNode * sourceNode, ConnectableNode * destNode, NodeConnection::ConnectionType connectionType)
{
	for (int i = connections.size(); --i >= 0;)
	{
		NodeConnection * c = connections.getUnchecked(i);
		if (c->sourceNode == sourceNode && c->destNode == destNode && c->connectionType == connectionType) return c;
	}

	return nullptr;
}

Array<NodeConnection*> NodeContainer::getAllConnectionsForNode(ConnectableNode * node)
{
	Array<NodeConnection*> result;
	for (auto &connection : connections)
	{
		if (connection->sourceNode == node || connection->destNode == node) result.add(connection);
	}

	return result;
}

NodeConnection * NodeContainer::addConnection(ConnectableNode * sourceNode, ConnectableNode * destNode, NodeConnection::ConnectionType connectionType)
{
	if (getConnectionBetweenNodes(sourceNode, destNode, connectionType) != nullptr)
	{
		//connection already exists
		DBG("Connection already exists");
		return nullptr;
	}

	NodeConnection * c = new NodeConnection(sourceNode, destNode, connectionType);
	connections.add(c);
	c->addConnectionListener(this);

	// DBG("Dispatch connection Added from NodeManager");
	nodeContainerListeners.call(&NodeContainerListener::connectionAdded, c);

	return c;
}


bool NodeContainer::removeConnection(NodeConnection * c)
{
	if (c == nullptr) return false;
	c->removeConnectionListener(this);

	connections.removeObject(c);

	nodeContainerListeners.call(&NodeContainerListener::connectionRemoved, c);

	return true;
}



//From NodeBase Listener
void NodeContainer::askForRemoveNode(ConnectableNode * node)
{
	removeNode((NodeBase*)node);
}


void NodeContainer::askForRemoveConnection(NodeConnection *connection)
{
	removeConnection(connection);
}

ConnectableNodeUI * NodeContainer::createUI()
{
	return new NodeContainerUI(this);
}

AudioProcessorGraph::Node * NodeContainer::getAudioNode(bool isInput)
{
	if (isInput)
	{
		return containerInNode == nullptr ? nullptr : containerInNode->getAudioNode();
	}
	else
	{
		return containerOutNode == nullptr ? nullptr : containerOutNode->getAudioNode();
	}
}



void NodeContainer::removeIllegalConnections() {
	//TODO synchronize this and implement it for data
	// it's not indispensable
	if (NodeManager::getInstanceWithoutCreating() != nullptr)
	{
		jassert(!NodeManager::getInstance()->audioGraph.removeIllegalConnections());
	}
}

