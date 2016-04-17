/*
 ==============================================================================

 NodeConnection.cpp
 Created: 7 Mar 2016 12:39:02pm
 Author:  bkupe

 ==============================================================================
 */

#include "NodeConnection.h"
#include "NodeManager.h"

NodeConnection::NodeConnection(NodeManager * nodeManager,uint32 connectionId, NodeBase * sourceNode, NodeBase * destNode, ConnectionType connectionType) :
nodeManager(nodeManager), connectionId(connectionId), sourceNode(sourceNode), destNode(destNode), connectionType(connectionType)
{

    // init with all possible Audio connections
    if(connectionType==AUDIO){
        int maxCommonAudiConnections = jmin(sourceNode->audioProcessor->getTotalNumOutputChannels() , destNode->audioProcessor->getTotalNumInputChannels());
        for( int i = 0 ; i <maxCommonAudiConnections ; i ++){
            addAudioGraphConnection(i, i);
        }
    }
}

NodeConnection::~NodeConnection()
{
    if(connectionType==AUDIO){removeAllAudioGraphConnections();}
    dataConnections.clear();
    sourceNode = nullptr;
    destNode = nullptr;
}

void NodeConnection::addAudioGraphConnection(uint32 sourceChannel, uint32 destChannel)
{
    audioConnections.add(AudioConnection(sourceChannel,destChannel));
    nodeManager->audioGraph.addConnection(sourceNode->nodeId, sourceChannel, destNode->nodeId, destChannel);
}

void NodeConnection::removeAudioGraphConnection(uint32 sourceChannel, uint32 destChannel)
{
    nodeManager->audioGraph.removeConnection(sourceNode->nodeId, sourceChannel, destNode->nodeId, destChannel);
    audioConnections.removeAllInstancesOf(AudioConnection(sourceChannel,destChannel));
}
void NodeConnection::removeAllAudioGraphConnections()
{
    for(auto c:audioConnections){
        nodeManager->audioGraph.removeConnection(sourceNode->nodeId, c.first, destNode->nodeId, c.second);
    }
    audioConnections.clear();

}
void NodeConnection::addDataGraphConnection(DataProcessor::Data * sourceData, DataProcessor::Data * destData)
{
	DataProcessorGraph::Connection * c = nodeManager->dataGraph.addConnection(sourceData, destData);
    dataConnections.add(c);
}

void NodeConnection::removeDataGraphConnection(DataProcessor::Data * sourceData, DataProcessor::Data * destData)
{
	DataProcessorGraph::Connection * c = nodeManager->dataGraph.getConnectionBetween(sourceData, destData);
    dataConnections.removeAllInstancesOf(c);
	nodeManager->dataGraph.removeConnection(c);
}

void NodeConnection::removeAllDataGraphConnections()
{
	for (auto &c : dataConnections) {
		nodeManager->dataGraph.removeConnection(c);
	}
	dataConnections.clear();
}


void NodeConnection::remove()
{
    listeners.call(&NodeConnection::Listener::askForRemoveConnection,this);
}

var NodeConnection::getJSONData()
{
	var data(new DynamicObject());
	data.getDynamicObject()->setProperty("srcNodeId", (int)sourceNode->nodeId);
	data.getDynamicObject()->setProperty("dstNodeId", (int)destNode->nodeId);
	data.getDynamicObject()->setProperty("connectionType", (int)connectionType);

	// TODO embed routing info

	return data;
}

void NodeConnection::loadJSONData(var data)
{
}
