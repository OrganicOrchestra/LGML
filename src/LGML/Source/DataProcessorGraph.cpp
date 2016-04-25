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
	thread.startThread();
}

DataProcessorGraph::~DataProcessorGraph()
{
	thread.stopThread(0);
    clear();
}


DataProcessorGraph::Connection::Connection(Data * sourceData, Data * destData) noexcept
    : sourceData(sourceData), destData(destData)
{
    if(sourceData != nullptr) sourceData->addDataListener(this);
}

void DataProcessorGraph::Connection::dataChanged(Data * d)
{
    if (destData != nullptr) destData->updateFromSourceData(d);
}


void DataProcessorGraph::clear()
{
    processors.clear();
    connections.clear();

}

DataProcessorGraph::Connection * DataProcessorGraph::getConnectionBetween(Data * sourceData, Data * destData) const
{
    for (int i = connections.size(); --i >= 0;)
    {
        Connection * c = connections.getUnchecked(i);
        if (c->sourceData == sourceData && c->destData == destData)
        {
            return c;
        }
    }

    return nullptr;
}


bool DataProcessorGraph::canConnect(Data * sourceData, Data * destData) const
{
    if (sourceData == nullptr || destData == nullptr)
        return false;

    /*
    if (source->getInputDataType(sourceDataName, sourceElementName) != dest->getOutputDataType(destDataName, destElementName)) return false;

    return getConnectionBetween(sourceNodeId, sourceDataName, sourceElementName,
        destNodeId, destDataName,destElementName) == nullptr;
        */
    return true;
}


DataProcessorGraph::Connection * DataProcessorGraph::addConnection(Data * sourceData, Data * destData)
{
    if (!canConnect(sourceData,destData))
        return nullptr;

    /*
    GraphRenderingOps::ConnectionSorter sorter;
    connections.addSorted(sorter, new Connection(sourceNodeId, sourceChannelIndex,
        destNodeId, destChannelIndex));
        */

    Connection * c = new Connection(sourceData, destData);
    connections.add(c);

    //triggerAsyncUpdate();
    return c;
}

void DataProcessorGraph::removeConnection(int index)
{
    connections.remove(index);
    //triggerAsyncUpdate();
}

void DataProcessorGraph::removeConnection(Connection * c)
{
    connections.removeObject(c, true);
}

