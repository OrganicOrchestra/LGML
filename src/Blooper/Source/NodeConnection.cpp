/*
  ==============================================================================

    NodeConnection.cpp
    Created: 7 Mar 2016 12:39:02pm
    Author:  bkupe

  ==============================================================================
*/

#include "NodeConnection.h"
#include "NodeManager.h"

NodeConnection::NodeConnection(uint32 connectionId, NodeBase * sourceNode, NodeBase * destNode, ConnectionType connectionType) :
	connectionId(connectionId), sourceNode(sourceNode), destNode(destNode), connectionType(connectionType)
{

}

NodeConnection::~NodeConnection()
{
	sourceNode = nullptr;
	destNode = nullptr;
}

void NodeConnection::addAudioGraphConnection(uint32 sourceChannel, uint32 destChannel)
{

}

void NodeConnection::removeAudioGraphConnection(uint32 sourceChannel, uint32 destChannel)
{

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
