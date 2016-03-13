/*
  ==============================================================================

    DataProcessorGraph.cpp
    Created: 3 Mar 2016 1:52:48pm
    Author:  bkupe

  ==============================================================================
*/

#include "DataProcessorGraph.h"


DataProcessorGraph::DataProcessorGraph()
{
}

DataProcessorGraph::~DataProcessorGraph()
{
	clear();
}


DataProcessorGraph::Connection::Connection(const uint32 sourceID, const String sourceDataName, const String sourceElementName,
	const uint32 destID, const String destDataName, const String destElementName) noexcept
	: sourceNodeId(sourceID), sourceDataName(sourceDataName),sourceElementName(sourceElementName),
	destNodeId(destID), destDataName(destDataName), destElementName(destElementName)
{
}

//==============================================================================
DataProcessorGraph::Node::Node(const uint32 nodeID, DataProcessor* const p) noexcept
	: nodeId(nodeID), processor(p)
{
	jassert(processor != nullptr);
}

void DataProcessorGraph::Node::setParentGraph(DataProcessorGraph *) const
{
	//??
}


void DataProcessorGraph::clear()
{
	nodes.clear();
	//connections.clear();
	//triggerAsyncUpdate();
}

DataProcessorGraph::Node * DataProcessorGraph::getNodeForId(const uint32 nodeId) const
{
	for (int i = nodes.size(); --i >= 0;)
		if (nodes.getUnchecked(i)->nodeId == nodeId)
			return nodes.getUnchecked(i);
    
    
    // TODO @ben do we handle wrong ids?
    jassertfalse;
    return nullptr;
}

DataProcessorGraph::Node * DataProcessorGraph::addNode(DataProcessor * newProcessor, uint32 nodeId)
{
	if (newProcessor == nullptr)
	{
		jassertfalse;
		return nullptr;
	}

	for (int i = nodes.size(); --i >= 0;)
	{
		if (nodes.getUnchecked(i)->getProcessor() == newProcessor)
		{
			jassertfalse; // Cannot add the same object to the graph twice!
			return nullptr;
		}
	}

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


	Node* n = new Node(nodeId, newProcessor);
	nodes.add(n);
	//triggerAsyncUpdate();

	n->setParentGraph(this);
	return n;
}

bool DataProcessorGraph::removeNode(uint32 nodeId)
{
	disconnectNode(nodeId);

	for (int i = nodes.size(); --i >= 0;)
	{
		if (nodes.getUnchecked(i)->nodeId == nodeId)
		{
			nodes.remove(i);
			//triggerAsyncUpdate();

			return true;
		}
	}

	return false;
}

DataProcessorGraph::Connection * DataProcessorGraph::getConnectionBetween(uint32 sourceNodeId, 
	String sourceDataName,
	String sourceElementName,
	uint32 destNodeId, 
	String destDataName,
	String destElementName
	) const
{
	for (int i = connections.size(); --i >= 0;)
	{
		Connection * c = connections.getUnchecked(i);
		if (c->sourceNodeId == sourceNodeId
			&& c->sourceDataName == sourceDataName
			&& c->sourceElementName == sourceElementName
			&& c->destNodeId == destNodeId
			&& c->destDataName == destDataName
			&& c->destElementName == destElementName)
		{
			return c;
		}
	}

	return nullptr;

	//const Connection c(sourceNodeId, sourceChannelIndex, destNodeId, destChannelIndex);
	//GraphRenderingOps::ConnectionSorter sorter;
	//return connections[connections.indexOfSorted(sorter, &c)];
}

bool DataProcessorGraph::isConnected(uint32 possibleSourceNodeId, uint32 possibleDestNodeId) const
{
	for (int i = connections.size(); --i >= 0;)
	{
		const Connection* const c = connections.getUnchecked(i);

		if (c->sourceNodeId == possibleSourceNodeId
			&& c->destNodeId == possibleDestNodeId)
		{
			return true;
		}
	}

	return false;
}

bool DataProcessorGraph::canConnect(uint32 sourceNodeId,
	String sourceDataName,
	String sourceElementName,
	uint32 destNodeId,
	String destDataName,
	String destElementName) const
{
	if (sourceDataName.isEmpty()
		|| destDataName.isEmpty()
		|| sourceNodeId == destNodeId
		)
		return false;

	Node* const source = getNodeForId(sourceNodeId);
	Node* const dest = getNodeForId(destNodeId);

	if (source->getInputDataType(sourceDataName, sourceElementName) != dest->getOutputDataType(destDataName, destElementName)) return false;

	return getConnectionBetween(sourceNodeId, sourceDataName, sourceElementName,
		destNodeId, destDataName,destElementName) == nullptr;
}


DataProcessorGraph::Connection * DataProcessorGraph::addConnection(uint32 sourceNodeId, String sourceDataName, String sourceElementName, uint32 destNodeId, String destDataName, String destElementName)
{
	if (!canConnect(sourceNodeId, sourceDataName, sourceElementName, destNodeId,destDataName,destElementName))
		return nullptr;

	/*
	GraphRenderingOps::ConnectionSorter sorter;
	connections.addSorted(sorter, new Connection(sourceNodeId, sourceChannelIndex,
		destNodeId, destChannelIndex));
		*/

	Connection * c = new Connection(sourceNodeId, sourceDataName, sourceElementName, destNodeId, destDataName, destElementName);
	connections.add(c);

	//triggerAsyncUpdate();
	return c;
}

void DataProcessorGraph::removeConnection(int index)
{
	connections.remove(index);
	//triggerAsyncUpdate();
}

bool DataProcessorGraph::removeConnection(uint32 sourceNodeId, String sourceDataName, String sourceElementName, uint32 destNodeId, String destDataName, String destElementName)
{
	bool doneAnything = false;

    for (int i = connections.size(); --i >= 0;)
    {
        const Connection* const c = connections.getUnchecked(i);

		if (c->sourceNodeId == sourceNodeId
			&& c->sourceDataName == sourceDataName
			&& c->sourceElementName == sourceElementName
			&& c->destNodeId == destNodeId
			&& c->destDataName == destDataName
			&& c->destElementName == destElementName)
		{
            removeConnection (i);
            doneAnything = true;
        }
    }

    return doneAnything;
}


bool DataProcessorGraph::disconnectNode(uint32 nodeId)
{
	bool doneAnything = false;

	for (int i = connections.size(); --i >= 0;)
	{
		const Connection* const c = connections.getUnchecked(i);

		if (c->sourceNodeId == nodeId || c->destNodeId == nodeId)
		{
			removeConnection(i);
			doneAnything = true;
		}
	}

	return doneAnything;
}
