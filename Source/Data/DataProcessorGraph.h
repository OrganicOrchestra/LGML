/*
 ==============================================================================

 Copyright © Organic Orchestra, 2017

 This file is part of LGML. LGML is a software to manipulate sound in realtime

 This program is free software; you can redistribute it and/or modify
 it under the terms of the GNU General Public License as published by
 the Free Software Foundation (version 3 of the License).

 This program is distributed in the hope that it will be useful,
 but WITHOUT ANY WARRANTY; without even the implied warranty of
 MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.

 ==============================================================================
 */

#ifndef DATAPROCESSORGRAPH_H_INCLUDED
#define DATAPROCESSORGRAPH_H_INCLUDED

#include "Data.h"//keep
class NodeBase;
/*
DataProcessoGraph handle a graph of DataProcessorGraph::Node,
    each Node refer to a dataProcessor and allow connections between them

*/
class DataProcessorGraph
{

public:
    DataProcessorGraph();
    ~DataProcessorGraph();

    class DataThread : public Thread
    {
    public:

        DataThread() : Thread ("dataThread") {}
        virtual ~DataThread();
        virtual void run() override;
    };

    DataThread thread;

    class Connection : public Data::DataListener
    {
    public:
        //==============================================================================
        Connection (Data* sourceData, Data* destData) noexcept;
        virtual ~Connection()
        {
            if (sourceData != nullptr) sourceData->removeDataListener (this);

            if (destData != nullptr) destData->removeDataListener (this);
        }

        Data* sourceData;
        Data* destData;

    private:
        //==============================================================================
        JUCE_LEAK_DETECTOR (Connection)

        // Inherited via DataListener
        virtual void dataChanged (Data*) override;
    };




    void clear();
    int getNumConnections() const { return connections.size(); }
    const Connection* getConnection (int index) const { return connections[index]; }
    Connection* getConnectionBetween (Data* sourceData, Data* destData) const;

    bool canConnect (Data* sourceData, Data* destData) const;

    Connection* addConnection (Data* sourceData, Data* destData);

    void removeConnection (int index);
    void removeConnection (Connection* connection);

private:
    Array<NodeBase*> processors;
    OwnedArray<Connection> connections;
    uint32 lastNodeId;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (DataProcessorGraph)
};


#endif  // DATAPROCESSORGRAPH_H_INCLUDED
