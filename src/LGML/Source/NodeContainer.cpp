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

NodeContainer::NodeContainer(const String &name) :
	parentNodeContainer(nullptr),
	containerInNode(nullptr),
	containerOutNode(nullptr),
	ConnectableNode(name, NodeType::ContainerType,false)
{
	canHavePresets = false;
	saveAndLoadRecursiveData = false;
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

	if (recreateContainerNodes && parentNodeContainer != nullptr)
	{
		containerInNode = (ContainerInNode *)addNode(new ContainerInNode());
		containerOutNode = (ContainerOutNode *)addNode(new ContainerOutNode());

		containerInNode->xPosition->setValue(150);
		containerInNode->yPosition->setValue(100);
		containerOutNode->xPosition->setValue(450);
		containerOutNode->yPosition->setValue(100);

		containerInNode->addRMSListener(this);
		containerOutNode->addRMSListener(this);

		//maybe keep it ?
		addConnection(containerInNode, containerOutNode, NodeConnection::ConnectionType::AUDIO);
	}

}


void NodeContainer::setParentNodeContainer(NodeContainer * _parentNodeContainer)
{
	parentNodeContainer = _parentNodeContainer;
	clear(true);
}

ConnectableNode * NodeContainer::addNode(NodeType nodeType)
{
	ConnectableNode * n = NodeFactory::createNode(nodeType);
	return addNode(n);
}

ConnectableNode * NodeContainer::addNode(ConnectableNode * n)
{
	nodes.add(n);
	if (n->type == NodeType::ContainerType)
	{
		nodeContainers.add((NodeContainer *)n);
		((NodeContainer *)n)->setParentNodeContainer(this);
		DBG("Check containerIn Node : " << String(((NodeContainer *)n)->containerInNode != nullptr));
	}

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

	n->clear();
	n->removeFromAudioGraph();

	if (n->type == NodeType::ContainerType) nodeContainers.removeObject((NodeContainer*)n);

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
	var data = ConnectableNode::getJSONData();
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
	clear(false);

	Array<var> * nodesData = data.getProperty("nodes", var()).getArray();
	for (var &nData : *nodesData)
	{
		NodeType nodeType = NodeFactory::getTypeFromString(nData.getProperty("nodeType", var()));

		ConnectableNode * node = addNode(nodeType);
		if (node->type == NodeType::ContainerInType) containerInNode = (ContainerInNode *)node;
		else if (node->type == NodeType::ContainerOutType) containerOutNode = (ContainerOutNode *)node;

		node->loadJSONData(nData);
	}

	Array<var> * connectionsData = data.getProperty("connections", var()).getArray();

	if (connectionsData)
	{
		for (var &cData : *connectionsData)
		{
			NodeBase * srcNode = (NodeBase *)getNodeForName(cData.getDynamicObject()->getProperty("srcNode").toString());
			NodeBase * dstNode = (NodeBase *)getNodeForName(cData.getDynamicObject()->getProperty("dstNode").toString());

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
	ConnectableNode * tSourceNode = (sourceNode->type == ContainerType) ? ((NodeContainer *)sourceNode)->containerOutNode : sourceNode;
	ConnectableNode * tDestNode = (destNode->type == ContainerType) ? ((NodeContainer *)destNode)->containerInNode : destNode;

	for (int i = connections.size(); --i >= 0;)
	{
		NodeConnection * c = connections.getUnchecked(i);
		if (c->sourceNode == tSourceNode && c->destNode == tDestNode && c->connectionType == connectionType) return c;
	}

	return nullptr;
}

Array<NodeConnection*> NodeContainer::getAllConnectionsForNode(ConnectableNode * node)
{
	Array<NodeConnection*> result;
	ConnectableNode * tSourceNode = (node->type == ContainerType) ? ((NodeContainer *)node)->containerOutNode : node;
	ConnectableNode * tDestNode = (node->type == ContainerType) ? ((NodeContainer *)node)->containerInNode : node;

	for (auto &connection : connections)
	{
		if (connection->sourceNode == tSourceNode || connection->destNode == tDestNode) result.add(connection);
	}

	return result;
}

NodeConnection * NodeContainer::addConnection(NodeBase * sourceNode, NodeBase * destNode, NodeConnection::ConnectionType connectionType)
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

void NodeContainer::RMSChanged(ConnectableNode * node, float _rmsInValue, float _rmsOutValue)
{
	if (node == containerInNode) rmsInValue = _rmsInValue;
	else if (node == containerOutNode) rmsOutValue = _rmsOutValue;

    rmsListeners.call(&ConnectableNode::RMSListener::RMSChanged, this, rmsInValue, rmsOutValue);
}

ConnectableNodeUI * NodeContainer::createUI()
{
	return new NodeContainerUI(this);
}


bool NodeContainer::hasDataInputs()
{
	return containerInNode != nullptr ? containerInNode->hasDataInputs() : false;
}

bool NodeContainer::hasDataOutputs()
{
	return containerOutNode != nullptr ? containerOutNode->hasDataOutputs() : false;
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
