/*
 ==============================================================================

 NodeConnection.cpp
 Created: 7 Mar 2016 12:39:02pm
 Author:  bkupe

 ==============================================================================
 */

#include "NodeConnection.h"
#include "NodeManager.h"

NodeConnection::NodeConnection(ConnectableNode * sourceNode, ConnectableNode * destNode, NodeConnection::ConnectionType connectionType, bool _isGhostConnection) :
sourceNode(sourceNode),
destNode(destNode),
connectionType(connectionType),
isGhostConnection(_isGhostConnection)
{

    // init with all possible Audio connections
    if(connectionType == AUDIO){
		int maxCommonAudioConnections = jmin(sourceNode->getAudioNode(true)->getProcessor()->getTotalNumOutputChannels(), destNode->getAudioNode(false)->getProcessor()->getTotalNumInputChannels());
		for (int i = 0; i < maxCommonAudioConnections; i++) {
			addAudioGraphConnection(i, i);
		}

    }

    if (sourceNode->type != NodeType::ContainerType) {
        ((NodeBase*)sourceNode)->addNodeBaseListener(this);
    }

    if (destNode->type != NodeType::ContainerType) {
        ((NodeBase*)destNode)->addNodeBaseListener(this);
    }


}

NodeConnection::~NodeConnection()
{
    if(connectionType==AUDIO){
        removeAllAudioGraphConnections();
    }

    dataConnections.clear();
    if(sourceNode.get()){
        if (sourceNode->type != NodeType::ContainerType && dynamic_cast<NodeBase *>(sourceNode.get()) != nullptr)
        {
            ((NodeBase*)sourceNode.get())->removeNodeBaseListener(this);
        }
    }

    if(destNode.get()){
        if(destNode->type!=NodeType::ContainerType && dynamic_cast<NodeBase *>(destNode.get()) != nullptr){
            ((NodeBase*)destNode.get())->removeNodeBaseListener(this);
        }
    }
}

bool NodeConnection::addAudioGraphConnection(uint32 sourceChannel, uint32 destChannel)
{

	bool result = true;
	if (!isGhostConnection)
	{
		if (NodeManager::getInstanceWithoutCreating() != nullptr)
		{
			result = NodeManager::getInstance()->audioGraph.addConnection(sourceNode->getAudioNode(false)->nodeId, sourceChannel, destNode->getAudioNode(true)->nodeId, destChannel);
		} else
		{
			result = false;
		}
	}

	if (result)
    {
        AudioConnection ac = AudioConnection(sourceChannel, destChannel);
        audioConnections.add(ac);
        listeners.call(&Listener::connectionAudioLinkAdded, ac);
    }
    return result;
}

void NodeConnection::removeAudioGraphConnection(uint32 sourceChannel, uint32 destChannel, bool keepInGhost)
{
    AudioConnection ac = AudioConnection(sourceChannel, destChannel);
	if (!isGhostConnection)
	{
		if (NodeManager::getInstanceWithoutCreating() != nullptr) NodeManager::getInstance()->audioGraph.removeConnection(sourceNode->getAudioNode(false)->nodeId, sourceChannel, destNode->getAudioNode(true)->nodeId, destChannel);
	}

	audioConnections.removeAllInstancesOf(ac);
    if (keepInGhost) ghostConnections.add(ac);
    listeners.call(&Listener::connectionAudioLinkRemoved, ac);


}
void NodeConnection::removeAllAudioGraphConnections()
{
    for(auto c:audioConnections){
        removeAudioGraphConnection(c.first,c.second,false);
    }

    audioConnections.clear();

}

void NodeConnection::removeAllAudioGraphConnectionsForChannel(int channel, bool isSourceChannel,bool keepInGhost)
{
    Array<AudioConnection> connectionsToRemove;

    for (auto c : audioConnections) {
        if ((isSourceChannel && c.first == channel) || (!isSourceChannel && c.second == channel))
        {
            connectionsToRemove.add(c);
        }
    }

    for(auto &tc : connectionsToRemove) removeAudioGraphConnection(tc.first, tc.second,keepInGhost);


}

void NodeConnection::addDataGraphConnection(Data * sourceData, Data * destData)
{
    DataProcessorGraph::Connection * c = NodeManager::getInstance()->dataGraph.addConnection(sourceData, destData);
    dataConnections.add(c);
    listeners.call(&Listener::connectionDataLinkAdded, c);
}

void NodeConnection::removeDataGraphConnection(Data * sourceData, Data * destData)
{
    DataProcessorGraph::Connection * c = NodeManager::getInstance()->dataGraph.getConnectionBetween(sourceData, destData);
    dataConnections.removeAllInstancesOf(c);
    listeners.call(&Listener::connectionDataLinkRemoved, c);
    NodeManager::getInstance()->dataGraph.removeConnection(c);

}

void NodeConnection::removeAllDataGraphConnections()
{
    while(dataConnections.size() > 0)
    {
        removeDataGraphConnection(dataConnections[0]->sourceData,dataConnections[0]->destData);
    }

    //useless ?
    dataConnections.clear();
}


void NodeConnection::removeAllDataGraphConnectionsForData(Data * data, bool isSourceData)
{
    Array<DataProcessorGraph::Connection *> connectionsToRemove;

    for (auto &c : dataConnections) {
        if ((isSourceData && c->sourceData == data) || (!isSourceData && c->destData == data))
        {
            connectionsToRemove.add(c);
        }
    }

    for(auto &tc : connectionsToRemove) removeDataGraphConnection(tc->sourceData, tc->destData);

}



void NodeConnection::remove()
{
    listeners.call(&NodeConnection::Listener::askForRemoveConnection,this);
}

void NodeConnection::audioInputAdded(NodeBase * n, int channel)
{
    //DBG("Audio Input Added " << channel << " ( " << ghostConnections.size() << " ghosts )");

    Array<AudioConnection> connectionsToAdd;
    for (auto &c : ghostConnections)
    {
        if (n == destNode && c.second == channel) connectionsToAdd.add(c);
    }

    //DBG(" >> connections to add : " << connectionsToAdd.size());
    for (auto &c : connectionsToAdd)
    {
        bool result = addAudioGraphConnection(c.first, c.second);
        if (result) ghostConnections.removeAllInstancesOf(c);
    }
}

void NodeConnection::audioOutputAdded(NodeBase * n, int channel)
{
    //DBG("Audio Output added " << channel << " ( " << ghostConnections.size() << " ghosts )");
    Array<AudioConnection> connectionsToAdd;
    for (auto &c : ghostConnections)
    {
        if (n == sourceNode && c.first == channel) connectionsToAdd.add(c);
    }

    for (auto &c : connectionsToAdd)
    {
        bool result = addAudioGraphConnection(c.first, c.second);
        if (result) ghostConnections.removeAllInstancesOf(c);
    }
}

void NodeConnection::audioInputRemoved(NodeBase * n, int channel)
{
    //DBG("Audio Input removed " << channel);
    if (n == destNode)
    {
        removeAllAudioGraphConnectionsForChannel(channel, false,true);
        //if (n->getTotalNumInputChannels() == 0) remove(); //not that useful with ghost support
    }
}

void NodeConnection::audioOutputRemoved(NodeBase * n, int channel)
{
    //DBG("Audio Output Removed "  << channel);
    if (n == sourceNode)
    {
        removeAllAudioGraphConnectionsForChannel(channel, true,true);
        //if (n->getTotalNumOutputChannels() == 0) remove(); //not useful with ghost support
    }
}


void NodeConnection::dataInputRemoved(NodeBase * n, Data * d)
{
    if (n == destNode)
    {
        removeAllDataGraphConnectionsForData(d, false);
        //if (n->getTotalNumInputData() == 0) remove();
    }
}

void NodeConnection::dataOutputRemoved(NodeBase * n , Data * d)
{
    if (n == sourceNode)
    {
        removeAllDataGraphConnectionsForData(d, true);
        //if (n->getTotalNumOutputData() == 0) remove();
    }
}

var NodeConnection::getJSONData()
{
    var data(new DynamicObject());

    ConnectableNode * tSource = sourceNode;
    if (sourceNode->type == ContainerOutType) tSource = ((ContainerOutNode *)sourceNode.get())->parentNodeContainer;

    ConnectableNode * tDest = destNode;
    if(destNode->type == ContainerInType) tDest = ((ContainerInNode *)destNode.get())->parentNodeContainer;

    data.getDynamicObject()->setProperty("srcNode", tSource->shortName);
    data.getDynamicObject()->setProperty("dstNode", tDest->shortName);
    data.getDynamicObject()->setProperty("connectionType", (int)connectionType);

    var links;
    if (isAudio())
    {
        for (auto &c : audioConnections)
        {
            var cObject(new DynamicObject());
            cObject.getDynamicObject()->setProperty("sourceChannel", c.first);
            cObject.getDynamicObject()->setProperty("destChannel", c.second);
            links.append(cObject);
        }
    }
    else
    {
        for (auto &c : dataConnections)
        {
            var cObject(new DynamicObject());
            cObject.getDynamicObject()->setProperty("sourceData", c->sourceData->name);
            cObject.getDynamicObject()->setProperty("destData", c->destData->name);
            links.append(cObject);
        }
    }

    data.getDynamicObject()->setProperty("links", links);

    return data;
}

void NodeConnection::loadJSONData(var data)
{
    //srcNodeId, destNodeId & connectionType set at creation, not in this load

    //DBG("Load JSON Data Node COnnection !");
    const Array<var> * links = data.getProperty("links",var()).getArray();

    if (links != nullptr)
    {
        if (isAudio())
        {
			removeAllAudioGraphConnections();
            for (var &linkVar : *links)
            {
                int sourceChannel = linkVar.getProperty("sourceChannel", var());
                int destChannel = linkVar.getProperty("destChannel", var());
                //DBG("Add from JSON, " << sourceChannel << " > " <<destChannel);

                addAudioGraphConnection(sourceChannel, destChannel);
            }
        }
        else
        {
            removeAllDataGraphConnections();
            for (auto &linkVar : *links)
            {
                String sourceName = linkVar.getProperty("sourceData", var());
                String destName = linkVar.getProperty("destData", var());
				if (!isGhostConnection)  addDataGraphConnection(sourceNode->getOutputDataByName(sourceName), destNode->getInputDataByName(destName));
            }
        }
    }
}
