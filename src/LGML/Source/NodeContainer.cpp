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


#include "DebugHelpers.h"
NodeContainer::NodeContainer(const String &name) :
containerInNode(nullptr),
containerOutNode(nullptr),
ConnectableNode(name, NodeType::ContainerType,false)
{
    saveAndLoadRecursiveData = false;
}


NodeContainer::~NodeContainer()
{
    //connections.clear();
    clear(false);
}

void NodeContainer::clear(bool recreateContainerNodes)
{

    while (connections.size() > 0)
    {
        connections[0]->remove();
    }

    while (nodes.size() > 0)
    {
        nodes[0]->remove();
    }

	containerInGhostConnections.clear();
	containerOutGhostConnections.clear();

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

        //maybe keep it ?
        addConnection(containerInNode, containerOutNode, NodeConnection::ConnectionType::AUDIO);
    }

	while (proxyParams.size() > 0)
	{
		removeParamProxy(proxyParams[0]);
	}

    ConnectableNode::clear();
}


ConnectableNode * NodeContainer::addNode(NodeType nodeType, const String &nodeName,bool callNodeAddedNow)
{
    ConnectableNode * n = NodeFactory::createNode(nodeType);
    return addNode(n,nodeName,callNodeAddedNow);
}

ConnectableNode * NodeContainer::addNode(ConnectableNode * n, const String &nodeName,bool callNodeAddedNow)
{
    nodes.add(n);
    n->setParentNodeContainer(this);

    if (n->type == NodeType::ContainerType)
    {
        nodeContainers.add((NodeContainer *)n);
        ((NodeContainer *)n)->clear(true);
        //DBG("Check containerIn Node : " << String(((NodeContainer *)n)->containerInNode != nullptr));
    }


    n->addNodeListener(this);
    String targetName = (nodeName.isNotEmpty())?nodeName:n->nameParam->stringValue();
    n->nameParam->setValue(getUniqueNameInContainer(targetName));

    addChildControllableContainer(n); //ControllableContainer
   if(callNodeAddedNow) nodeContainerListeners.call(&NodeContainerListener::nodeAdded, n);
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

ParameterProxy * NodeContainer::addParamProxy()
{
	ParameterProxy * p = new ParameterProxy();
	addParameter(p);
	proxyParams.add(p);
	nodeContainerListeners.call(&NodeContainerListener::paramProxyAdded, p);

	return p;
}

void NodeContainer::removeParamProxy(ParameterProxy * pp)
{
	proxyParams.removeAllInstancesOf(pp);
	removeControllable(pp);
	nodeContainerListeners.call(&NodeContainerListener::paramProxyRemoved, pp);
}
	

bool NodeContainer::loadPreset(PresetManager::Preset * preset)
{
    if(!ControllableContainer::loadPreset(preset)) return false;

//    for (auto &n : nodes) n->loadPresetWithName(preset->name);

    return true;
}

PresetManager::Preset* NodeContainer::saveNewPreset(const String & name)
{
    return ControllableContainer::saveNewPreset(name);
//    for (auto &n : nodes) n->saveNewPreset(name);
}

bool NodeContainer::saveCurrentPreset()
{
    if (!ControllableContainer::saveCurrentPreset()) return false;
    //@ben remove recursive for now (not useful and duplicating values...)
    //
//    for (auto &n : nodes) n->saveCurrentPreset();
    return true;
}

bool NodeContainer::resetFromPreset()
{
    if (!ControllableContainer::resetFromPreset()) return false;

//    for (auto &n : nodes) n->resetFromPreset();

    return true;
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

	var ghostConnectionsInData;
	for (auto &c : containerInGhostConnections)
	{
		ghostConnectionsInData.append(c->getJSONData());
	}
	data.getDynamicObject()->setProperty("ghostConnectionsIn", ghostConnectionsInData);
	

	var ghostConnectionsOutData;
	for (auto &c : containerOutGhostConnections)
	{
		ghostConnectionsOutData.append(c->getJSONData());
	}
	data.getDynamicObject()->setProperty("ghostConnectionsOut", ghostConnectionsOutData);


	var proxiesData;
	for (auto &pp : proxyParams)
	{
		proxiesData.append(pp->getJSONData());
	}

	data.getDynamicObject()->setProperty("proxies", proxiesData);

    return data;
}

void NodeContainer::loadJSONDataInternal(var data)
{
    clear(false);

    Array<var> * nodesData = data.getProperty("nodes", var()).getArray();
    if(nodesData!=nullptr){
        for (var &nData : *nodesData)
        {
            addNodeFromJSON(nData);
        }
    }

    Array<var> * connectionsData = data.getProperty("connections", var()).getArray();

    if (connectionsData)
    {
        for (var &cData : *connectionsData)
        {

            ConnectableNode * srcNode = (ConnectableNode*)(getNodeForName(cData.getDynamicObject()->getProperty("srcNode").toString())) ;
            ConnectableNode * dstNode = (ConnectableNode*)(getNodeForName(cData.getDynamicObject()->getProperty("dstNode").toString()));

            int cType = cData.getProperty("connectionType", var());

            if (srcNode && dstNode && isPositiveAndBelow(cType, (int)NodeConnection::ConnectionType::UNDEFINED)) {
                NodeConnection * c = addConnection(srcNode, dstNode, NodeConnection::ConnectionType(cType));
                // if c == null connection already exist, should never happen loading JSON but safer to check
                if(c){
                    c->loadJSONData(cData);
                }


            }
            else {
                // TODO nicely handle file format errors?

                if(srcNode==nullptr){
                    NLOG("loadJSON","no srcnode for shortName : "+cData.getDynamicObject()->getProperty("srcNode").toString());
                }
                if(dstNode==nullptr){
                    NLOG("loadJSON","no dstnode for shortName : "+cData.getDynamicObject()->getProperty("dstNode").toString());
                }
                LOG("Available Nodes in "+ shortName+" : ");
                for (auto &node : nodes)
                {
                    DBG(" > " + node->niceName+"//"+ node->shortName);
                }
                jassertfalse;
            }
        }
    }



	Array<var> * ghostConnectionInData = data.getProperty("ghostConnectionsIn", var()).getArray();

	if (ghostConnectionInData)
	{
		for (var &cData : *ghostConnectionInData)
		{
			ConnectableNode * srcNode = (ConnectableNode*)(getNodeForName(cData.getDynamicObject()->getProperty("srcNode").toString()));
			ConnectableNode * dstNode = (ConnectableNode*)(getNodeForName(cData.getDynamicObject()->getProperty("dstNode").toString()));

			int cType = cData.getProperty("connectionType", var());

			if (srcNode && dstNode && isPositiveAndBelow(cType, (int)NodeConnection::ConnectionType::UNDEFINED)) {
				NodeConnection * c = new NodeConnection(srcNode, dstNode, NodeConnection::ConnectionType(cType),true);
				c->loadJSONData(cData);
				containerInGhostConnections.add(c);
			}
		}
	}

	Array<var> * ghostConnectionOutData = data.getProperty("ghostConnectionsOut", var()).getArray();

	if (ghostConnectionOutData)
	{
		for (var &cData : *ghostConnectionOutData)
		{
			ConnectableNode * srcNode = (ConnectableNode*)(getNodeForName(cData.getDynamicObject()->getProperty("srcNode").toString()));
			ConnectableNode * dstNode = (ConnectableNode*)(getNodeForName(cData.getDynamicObject()->getProperty("dstNode").toString()));

			int cType = cData.getProperty("connectionType", var());

			if (srcNode && dstNode && isPositiveAndBelow(cType, (int)NodeConnection::ConnectionType::UNDEFINED)) {
				NodeConnection * c = new NodeConnection(srcNode, dstNode, NodeConnection::ConnectionType(cType), true);
				c->loadJSONData(cData);
				containerOutGhostConnections.add(c);
			}
		}
	}

   
	Array<var> * proxiesData = data.getProperty("proxies", var()).getArray();

	if (proxiesData)
	{
		for (var &pData : *proxiesData)
		{
			ParameterProxy * p = addParamProxy();
			p->loadJSONData(pData);
		}
	}


	removeIllegalConnections();
}

ConnectableNode * NodeContainer::addNodeFromJSON(var nodeData)
{
	Array<var> * params = nodeData.getDynamicObject()->getProperty("parameters").getArray();
	String sourceName = "";
	for (auto &v : *params)
	{

		if (v.getDynamicObject()->getProperty("controlAddress") == "/name")
		{
			sourceName = v.getDynamicObject()->getProperty("value").toString();
			break;
		}
	}

	
	NodeType nodeType = NodeFactory::getTypeFromString(nodeData.getProperty("nodeType", var()));
    ConnectableNode * node = addNode(nodeType, sourceName,false);
    String safeNodeName = node->niceName;

    if (node->type == NodeType::ContainerInType)
    {
        containerInNode = (ContainerInNode *)node;
        
    } else if (node->type == NodeType::ContainerOutType)
    {
        containerOutNode = (ContainerOutNode *)node;
    }

    
    node->loadJSONData(nodeData);
	node->nameParam->setValue(safeNodeName); //@martin new naming now takes into account the original node name

    nodeContainerListeners.call(&NodeContainerListener::nodeAdded, node);	

    return node;

}


NodeConnection * NodeContainer::getConnectionBetweenNodes(ConnectableNode * sourceNode, ConnectableNode * destNode, NodeConnection::ConnectionType connectionType)
{
    if(sourceNode==nullptr || destNode == nullptr){
        DBG("wrong Connection");
        return nullptr;
    }
    ConnectableNode * tSourceNode = (sourceNode->type == ContainerType) ? ((NodeContainer *)sourceNode)->containerOutNode : sourceNode;
    ConnectableNode * tDestNode = (destNode->type == ContainerType) ? ((NodeContainer *)destNode)->containerInNode : destNode;

    for(auto &c: connections)
	{
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

void NodeContainer::RMSChanged(ConnectableNode * node, float _rmsInValue, float _rmsOutValue)
{
    if (node == containerInNode) rmsInValue = _rmsInValue;
    else if (node == containerOutNode) rmsOutValue = _rmsOutValue;

    rmsListeners.call(&ConnectableNode::RMSListener::RMSChanged, this, rmsInValue, rmsOutValue);
}

void NodeContainer::onContainerParameterChanged(Parameter * p)
{
    ConnectableNode::onContainerParameterChanged(p);
    if (p == enabledParam)
    {

        bypassNode(!enabledParam->boolValue());

    }
}
void NodeContainer::bypassNode(bool bypass){
    if(bypass){
        jassert(containerInNode!=nullptr &&containerOutNode!=nullptr);
//        save old ones
        Array<NodeConnection*> connectionPointers;
        connectionPointers = getAllConnectionsForNode(containerInNode);
        containerInGhostConnections.clear();
        for(auto &c: connectionPointers){

			NodeConnection * nc = new NodeConnection(c->sourceNode, c->destNode, c->connectionType, true);
			nc->loadJSONData(c->getJSONData());
			containerInGhostConnections.add(nc);

		}
        for(auto & c:connectionPointers){
			removeConnection(c);
		}


        containerOutGhostConnections.clear();
        connectionPointers = getAllConnectionsForNode(containerOutNode);
        for(auto &c: connectionPointers){

			NodeConnection * nc = new NodeConnection(c->sourceNode, c->destNode, c->connectionType, true);
			nc->loadJSONData(c->getJSONData());
			containerOutGhostConnections.add(nc);
		}

        for(auto & c:connectionPointers){
			removeConnection(c);
		}

		// add a pass-thru
		addConnection(containerInNode, containerOutNode,NodeConnection::ConnectionType::AUDIO);
		addConnection(containerInNode, containerOutNode,NodeConnection::ConnectionType::DATA);

    }else
	{
        // remove pass thru
        Array<NodeConnection * > bypassConnection = getAllConnectionsForNode(containerInNode);
        jassert(bypassConnection.size()==2);
        for(auto & c:bypassConnection) {removeConnection(c);}
        bypassConnection = getAllConnectionsForNode(containerOutNode);
        jassert(bypassConnection.size()==0);

        for(auto & c:containerInGhostConnections){
            if(c->sourceNode!=nullptr&& c->destNode!=nullptr){
			NodeConnection *nc = addConnection(c->sourceNode, c->destNode, c->connectionType);
                if(nc){
                    nc->loadJSONData(c->getJSONData());

                }
            }
		}
        for(auto & c:containerOutGhostConnections){
            if(c->sourceNode!=nullptr&& c->destNode!=nullptr){
			NodeConnection *nc = addConnection(c->sourceNode, c->destNode, c->connectionType);
			if(nc)nc->loadJSONData(c->getJSONData());
            }
		}

        

    }
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

void NodeContainer::channelRMSInChanged(ConnectableNode * , float , int )
{
}

void NodeContainer::channelRMSOutChanged(ConnectableNode * , float , int )
{
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
