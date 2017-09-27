/* Copyright © Organic Orchestra, 2017
*
* This file is part of LGML.  LGML is a software to manipulate sound in realtime
*
* This program is free software; you can redistribute it and/or modify
* it under the terms of the GNU General Public License as published by
* the Free Software Foundation (version 3 of the License).
*
* This program is distributed in the hope that it will be useful,
* but WITHOUT ANY WARRANTY; without even the implied warranty of
* MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
*
*/

#include "DataProcessorGraph.h"

DataProcessorGraph::DataProcessorGraph()
{
    //    @ben not starting if not used, furthermore a while(true){} eats all the CPU for nothing
    //  thread.startThread();
}

DataProcessorGraph::~DataProcessorGraph()
{
    thread.signalThreadShouldExit();
    clear();
}


DataProcessorGraph::Connection::Connection (Data* sourceData, Data* destData) noexcept
    : sourceData (sourceData), destData (destData)
{
    if (sourceData != nullptr) sourceData->addDataListener (this);
}

void DataProcessorGraph::Connection::dataChanged (Data* d)
{
    if (destData != nullptr) destData->updateFromSourceData (d);
}



void DataProcessorGraph::clear()
{
    processors.clear();
    connections.clear();

}

DataProcessorGraph::Connection* DataProcessorGraph::getConnectionBetween (Data* sourceData, Data* destData) const
{
    for (int i = connections.size(); --i >= 0;)
    {
        Connection* c = connections.getUnchecked (i);

        if (c->sourceData == sourceData && c->destData == destData)
        {
            return c;
        }
    }

    return nullptr;
}


bool DataProcessorGraph::canConnect (Data* sourceData, Data* destData) const
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


DataProcessorGraph::Connection* DataProcessorGraph::addConnection (Data* sourceData, Data* destData)
{
    if (!canConnect (sourceData, destData))
        return nullptr;

    /*
    GraphRenderingOps::ConnectionSorter sorter;
    connections.addSorted(sorter, new Connection(sourceNodeId, sourceChannelIndex,
        destNodeId, destChannelIndex));
        */

    Connection* c = new Connection (sourceData, destData);
    connections.add (c);

    //triggerAsyncUpdate();
    return c;
}

void DataProcessorGraph::removeConnection (int index)
{
    connections.remove (index);
    //triggerAsyncUpdate();
}

void DataProcessorGraph::removeConnection (Connection* c)
{
    connections.removeObject (c, true);
}



DataProcessorGraph::DataThread::~DataThread()
{
    stopThread (100);
}

void DataProcessorGraph::DataThread::run()
{

    //  while (!threadShouldExit()) {
    //
    //  }
    DBG ("finish data thread");
}
