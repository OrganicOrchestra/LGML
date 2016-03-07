/*
  ==============================================================================

    NodeManager.h
    Created: 2 Mar 2016 8:33:39pm
    Author:  bkupe

  ==============================================================================
*/

#ifndef NODEMANAGER_H_INCLUDED
#define NODEMANAGER_H_INCLUDED
/*
Node Manager Contain all Node and synchronize building of audioGraph (AudioProcessorGraph) and DataGraph (DataProcessorGraph)

*/
#include "JuceHeader.h"

#ifdef JUCE_WINDOWS
#include "juce_audio_processors\juce_audio_processors.h"
#else
#include "juce_audio_processors.h"
#endif

#include "DataProcessorGraph.h"


#include "NodeFactory.h"
#include "NodeConnection.h"


class NodeManager: public NodeBase::Listener , public NodeConnection::Listener
{
	
public:
	NodeManager();
	~NodeManager();


	NodeFactory nodeFactory;


	AudioProcessorGraph audioGraph;
	DataProcessorGraph dataGraph;

	
	void clear();
	int getNumNodes() const noexcept { return nodes.size(); }

	NodeBase* getNode(const int index) const noexcept { return nodes[index]; }
	NodeBase* getNodeForId(const uint32 nodeId) const;
	NodeBase* addNode(NodeFactory::NodeType nodeType, uint32 nodeId = 0);
	bool removeNode(uint32 nodeId);

	NodeConnection * getConnection(const int index) const noexcept { return connections[index]; }
	NodeConnection * getConnectionForId(const uint32 connectionId) const;
	NodeConnection * getConnectionBetweenNodes(NodeBase * sourceNode, NodeBase * destNode, NodeConnection::ConnectionType connectionType);
	Array<NodeConnection *> getAllConnectionsForNode(NodeBase * node);

	NodeConnection * addConnection(NodeBase * sourceNode, NodeBase * destNode, NodeConnection::ConnectionType connectionType, uint32 connectionId = 0);
	bool removeConnection(uint32 connectionId);
	bool removeConnection(NodeConnection * c);

	//Listener
	class  Listener
	{
	public:
		/** Destructor. */
		virtual ~Listener() {}

		virtual void nodeAdded(NodeBase *) = 0;
		virtual void nodeRemoved(NodeBase *) = 0;

		virtual void connectionAdded(NodeConnection *) = 0;
		virtual void connectionEdited(NodeConnection *) = 0;
		virtual void connectionRemoved(NodeConnection *) = 0;
	};

	ListenerList<Listener> listeners;
	void addListener(Listener* newListener) { listeners.add(newListener); }
	void removeListener(Listener* listener) { listeners.remove(listener); }


private: 
	ReferenceCountedArray<NodeBase> nodes;
	uint32 lastNodeId;
	
	ReferenceCountedArray<NodeConnection> connections;
	uint32 lastConnectionId;

	JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(NodeManager)

	// Inherited via NodeBase::Listener
	virtual void askForRemoveNode(NodeBase *) override;

	// Inherited via NodeConnection::Listener
	virtual void connectionEdited(NodeConnection *) override;
	virtual void askForRemoveConnection(NodeConnection *) override;
};


#endif  // NODEMANAGER_H_INCLUDED
