/*
  ==============================================================================

    NodeManager.cpp
    Created: 2 Mar 2016 8:33:39pm
    Author:  bkupe

  ==============================================================================
*/

#include "NodeManager.h"
#include "DummyNode.h"


juce_ImplementSingleton(NodeManager);

NodeManager::NodeManager() :
    ControllableContainer("Node Manager")
{
	saveAndLoadRecursiveData = false;
    setCustomShortName("node");
}

NodeManager::~NodeManager()
{
    clear();

}

void NodeManager::clear()
{
   while(nodes.size())
        nodes[0]->remove();

    connections.clear();
    audioGraph.clear();
    dataGraph.clear();
    lastNodeId=0;
}

NodeBase * NodeManager::getNodeForId(const uint32 nodeId) const
{
    for (int i = nodes.size(); --i >= 0;)
        if (nodes.getUnchecked(i)->nodeId == nodeId)
            return nodes.getUnchecked(i);

    return nullptr;
}

NodeBase * NodeManager::addNode(NodeType nodeType, uint32 nodeId)
{
    if (nodeId == 0)
    {
        nodeId = ++lastNodeId;
    }
    else
    {
        // you can't add a node with an id that already exists in the graph..
        jassert(getNodeForId(nodeId) == nullptr);
        //DBG("Remove node because id already exists and pointer is null");
        removeNode(nodeId);

        if (nodeId > lastNodeId)
            lastNodeId = nodeId;
    }


    NodeBase * n = nodeFactory.createNode(this,nodeType, nodeId);
    nodes.add(n);
    n->addNodeListener(this);
	n->nameParam->setValue(getUniqueNameInContainer(n->nameParam->stringValue()));
    addChildControllableContainer(n); //ControllableContainer
    listeners.call(&NodeManager::Listener::nodeAdded,n);

    return n;
}



bool NodeManager::removeNode(uint32 nodeId)
{
    NodeBase * n = getNodeForId(nodeId);
    Array<NodeConnection *> relatedConnections = getAllConnectionsForNode(n);

    for (auto &connection : relatedConnections) removeConnection(connection);

    if (n == nullptr) return false;
    n->removeNodeListener(this);
    removeChildControllableContainer(n);

    listeners.call(&NodeManager::Listener::nodeRemoved, n);
    nodes.removeAllInstancesOf(n);
    audioGraph.removeNode(nodeId);

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
int NodeManager::getNumConnections(){
    return connections.size();
}

var NodeManager::getJSONData()
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
        connectionsData.append (c->getJSONData());
    }

    data.getDynamicObject()->setProperty("nodes", nodesData);
    data.getDynamicObject()->setProperty("connections",connectionsData);
    return data;
}

void NodeManager::loadJSONDataInternal(var data)
{
    clear();

    Array<var> * nodesData = data.getProperty("nodes", var()).getArray();
    for (var &nData : *nodesData)
    {
        NodeType nodeType = NodeFactory::getTypeFromString(nData.getProperty("nodeType",var()));
        int nodeId = nData.getProperty("nodeId", var());
        NodeBase* node = addNode(nodeType, nodeId);
        node->loadJSONData(nData);
    }

    Array<var> * connectionsData = data.getProperty("connections", var()).getArray();
    if (connectionsData)
    {
        for (var &cData : *connectionsData)
        {
            NodeBase * srcNode = getNodeForId((int)(cData.getProperty("srcNodeId", var())));
            NodeBase * dstNode = getNodeForId((int)(cData.getProperty("dstNodeId", var())));
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
    c->addConnectionListener(this);

   // DBG("Dispatch connection Added from NodeManager");
    listeners.call(&NodeManager::Listener::connectionAdded, c);

    return c;
}

bool NodeManager::removeConnection(uint32 connectionId)
{
    //DBG("Remove connection from Node Manager, dispatch connectionRemoved to UI");
    NodeConnection * c = getConnectionForId(connectionId);
    return removeConnection(c);
}

bool NodeManager::removeConnection(NodeConnection * c)
{
    if (c == nullptr) return false;
    c->removeConnectionListener(this);

    connections.removeObject(c);

    listeners.call(&NodeManager::Listener::connectionRemoved, c);

    return true;
}



//From NodeBase Listener
void NodeManager::askForRemoveNode(NodeBase * node)
{
    removeNode(node->nodeId);
}


void NodeManager::askForRemoveConnection(NodeConnection *connection)
{
    removeConnection(connection->connectionId);
}



void NodeManager::removeIllegalConnections(){
    //TODO synchronize this and implement it for data
    // it's not indispensable
    jassert(!audioGraph.removeIllegalConnections());
}

void NodeManager::updateAudioGraph(){
    AudioIODevice * ad  =getAudioDeviceManager().getCurrentAudioDevice();
    audioGraph.prepareToPlay(ad->getCurrentSampleRate(), ad->getDefaultBufferSize());
}
