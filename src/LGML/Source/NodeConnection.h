/*
  ==============================================================================

    NodeConnection.h
    Created: 7 Mar 2016 12:39:02pm
    Author:  bkupe

  ==============================================================================
*/

#ifndef NODECONNECTION_H_INCLUDED
#define NODECONNECTION_H_INCLUDED


#include "DataProcessorGraph.h"
class NodeManager;
class NodeBase;

class NodeConnection :
	public ReferenceCountedObject,
	public NodeBase::NodeBaseListener
{
public:
    enum ConnectionType
    {
        AUDIO, DATA, UNDEFINED
    };

    ConnectionType connectionType;

    bool isAudio() { return connectionType == ConnectionType::AUDIO; }
    bool isData() { return connectionType == ConnectionType::DATA; }

	ConnectableNode * sourceNode;
	ConnectableNode * destNode;

    typedef std::pair<int,int> AudioConnection;
    Array<AudioConnection> audioConnections;
    Array<DataProcessorGraph::Connection *> dataConnections;


    NodeConnection(ConnectableNode * sourceNode, ConnectableNode * destNode, ConnectionType connectionType);
    virtual ~NodeConnection();

    //Audio
    void addAudioGraphConnection(uint32 sourceChannel, uint32 destChannel);
    void removeAudioGraphConnection(uint32 sourceChannel, uint32 destChannel);
    void removeAllAudioGraphConnections();

	void removeAllAudioGraphConnectionsForChannel(int channel, bool isSourceChannel);

    //Data
    void addDataGraphConnection(Data * sourceData, Data * destData);
    void removeDataGraphConnection(Data * sourceData, Data * destData);
    void removeAllDataGraphConnections();

	void removeAllDataGraphConnectionsForData(Data *, bool isSourceData);

    void remove();

	virtual void audioInputRemoved(NodeBase *, int /* channel */) override;
	virtual void audioOutputRemoved(NodeBase *, int /* channel */) override;
	virtual void dataInputRemoved(NodeBase *, Data *) override;
	virtual void dataOutputRemoved(NodeBase *, Data *) override;


	// save / load

    var getJSONData();
    void loadJSONData(var data);

    //Listener
    class  Listener
    {
    public:
        /** Destructor. */
        virtual ~Listener() {}

		virtual void askForRemoveConnection(NodeConnection *) {}

		virtual void connectionDataLinkAdded(DataProcessorGraph::Connection * ) {}
        virtual void connectionDataLinkRemoved(DataProcessorGraph::Connection * ) {}

        virtual void connectionAudioLinkAdded(const AudioConnection &) {}
        virtual void connectionAudioLinkRemoved(const AudioConnection &) {}
    };

    ListenerList<Listener> listeners;
    void addConnectionListener(Listener* newListener) { listeners.add(newListener); }
    void removeConnectionListener(Listener* listener) { listeners.remove(listener); }


    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(NodeConnection)

};


#endif  // NODECONNECTION_H_INCLUDED
