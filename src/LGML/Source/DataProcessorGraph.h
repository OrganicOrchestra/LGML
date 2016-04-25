/*
  ==============================================================================

    DataProcessorGraph.h
    Created: 3 Mar 2016 1:52:48pm
    Author:  bkupe

  ==============================================================================
*/

#ifndef DATAPROCESSORGRAPH_H_INCLUDED
#define DATAPROCESSORGRAPH_H_INCLUDED

#include "JuceHeader.h"
#include "DataProcessor.h"


#include "DataProcessor.h"
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

		DataThread() : Thread("dataThread") {}
		virtual ~DataThread();
		virtual void run() override;
	};

	DataThread thread;

    class Connection : public Data::DataListener
    {
    public:
        //==============================================================================
        Connection(Data * sourceData, Data * destData) noexcept;
        virtual ~Connection()
        {
            if (sourceData != nullptr) sourceData->removeDataListener(this);
        }

        Data * sourceData;
        Data * destData;

    private:
        //==============================================================================
        JUCE_LEAK_DETECTOR(Connection)

            // Inherited via DataListener
            virtual void dataChanged(Data *) override;
    };




    void clear();
    int getNumConnections() const { return connections.size(); }
    const Connection* getConnection(int index) const { return connections[index]; }
    Connection* getConnectionBetween(Data * sourceData, Data * destData) const;

    bool canConnect(Data * sourceData, Data * destData) const;

    Connection * addConnection(Data * sourceData, Data * destData);

    void removeConnection(int index);
    void removeConnection(Connection * connection);

    private:
        Array<DataProcessor *> processors;
        OwnedArray<Connection> connections;
        uint32 lastNodeId;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(DataProcessorGraph)
};


#endif  // DATAPROCESSORGRAPH_H_INCLUDED
