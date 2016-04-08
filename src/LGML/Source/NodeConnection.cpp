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
void NodeConnection::addDataGraphConnection(const String &sourceDataName, const String &sourceElementName, const String &destDataName, const String &destElementName)
{
    DataProcessorGraph::Connection * c = nodeManager->dataGraph.addConnection(sourceNode->nodeId, sourceDataName, sourceElementName, destNode->nodeId, destDataName, destElementName);
    dataConnections.add(c);
}

void NodeConnection::removeDataGraphConnection(const String &sourceDataName, const String &sourceElementName, const String &destDataName, const String &destElementName)
{
    DataProcessorGraph::Connection * c = nodeManager->dataGraph.getConnectionBetween(sourceNode->nodeId, sourceDataName, sourceElementName, destNode->nodeId, destDataName, destElementName);
    dataConnections.removeAllInstancesOf(c);
    nodeManager->dataGraph.removeConnection(sourceNode->nodeId, sourceDataName, sourceElementName, destNode->nodeId, destDataName, destElementName);
}


void NodeConnection::remove()
{
    listeners.call(&NodeConnection::Listener::askForRemoveConnection,this);
}
