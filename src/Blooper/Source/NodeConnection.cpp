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
	sourceNode = nullptr;
	destNode = nullptr;
}

void NodeConnection::addAudioGraphConnection(uint32 sourceChannel, uint32 destChannel)
{
    nodeManager->audioGraph.addConnection(sourceNode->nodeId, sourceChannel, destNode->nodeId, destChannel);

}

void NodeConnection::removeAudioGraphConnection(uint32 sourceChannel, uint32 destChannel)
{
    nodeManager->audioGraph.removeConnection(sourceNode->nodeId, sourceChannel, destNode->nodeId, destChannel);
}

void NodeConnection::addDataGraphConnection(const String &sourceDataName, const String &sourceElementName, const String &destDataName, const String &destElementName)
{

}

void NodeConnection::removeDataGraphConnection(const String &sourceDataName, const String &sourceElementName, const String &destDataName, const String &destElementName)
{

}


void NodeConnection::remove()
{
	//manage clearing all inner audio or data connections/subconnections
	audioConnections.clear();
	dataConnections.clear();
}
