/*
  ==============================================================================

    NodeContainer.h
    Created: 18 May 2016 7:53:56pm
    Author:  bkupe

  ==============================================================================
*/

#ifndef NODECONTAINER_H_INCLUDED
#define NODECONTAINER_H_INCLUDED

#include "NodeBase.h"
#include "NodeConnection.h"
#include "NodeFactory.h"
#include "ConnectableNode.h"

//Listener
class  NodeContainerListener
{
public:
	/** Destructor. */
	virtual ~NodeContainerListener() {}

	virtual void nodeAdded(NodeBase *) = 0;
	virtual void nodeRemoved(NodeBase *) = 0;

	virtual void connectionAdded(NodeConnection *) = 0;
	virtual void connectionRemoved(NodeConnection *) = 0;
};


class NodeContainer : 
	public ConnectableNode,
	public ConnectableNode::ConnectableNodeListener, 
	public NodeConnection::Listener
{
public:
	NodeContainer(const String &name);
	virtual ~NodeContainer();

	Array<NodeBase *> nodes; //Not OwnedArray anymore because NodeBase is AudioProcessor, therefore owned by AudioProcessorGraph
	OwnedArray<NodeConnection> connections;

	NodeBase* getNode(const int index) const noexcept { return nodes[index]; }
	NodeBase* getNodeForId(const uint32 nodeId) const;
	NodeBase* addNode(NodeType nodeType, uint32 nodeId = 0);
	bool removeNode(NodeBase * n);

	NodeConnection * getConnection(const int index) const noexcept { return connections[index]; }
	NodeConnection * getConnectionForId(const uint32 connectionId) const;
	NodeConnection * getConnectionBetweenNodes(NodeBase * sourceNode, NodeBase * destNode, NodeConnection::ConnectionType connectionType);
	Array<NodeConnection *> getAllConnectionsForNode(NodeBase * node);

	NodeConnection * addConnection(NodeBase * sourceNode, NodeBase * destNode, NodeConnection::ConnectionType connectionType, uint32 connectionId = 0);
	bool removeConnection(uint32 connectionId);
	bool removeConnection(NodeConnection * c);
	void removeIllegalConnections();
	int getNumConnections();

	int getNumNodes() const noexcept { return nodes.size(); }

	//Container related
	
	//NodeContainer * addContainer();
	//void removeContainer(NodeContainer * c);


	//save / load
	var getJSONData() override;
	void loadJSONDataInternal(var data) override;

	void clear();

	// Inherited via NodeBase::Listener
	virtual void askForRemoveNode(ConnectableNode *) override;

	// Inherited via NodeConnection::Listener
	virtual void askForRemoveConnection(NodeConnection *) override;

	// Inherited via Listener
	virtual void connectionDataLinkAdded(DataProcessorGraph::Connection *) override {}
	virtual void connectionDataLinkRemoved(DataProcessorGraph::Connection *) override {}
	virtual void connectionAudioLinkAdded(const NodeConnection::AudioConnection &) override {}
	virtual void connectionAudioLinkRemoved(const NodeConnection::AudioConnection &) override {}

	ListenerList<NodeContainerListener> nodeContainerListeners;
	void addNodeContainerListener(NodeContainerListener* newListener) { nodeContainerListeners.add(newListener); }
	void removeNodeContainerListener(NodeContainerListener* listener) { nodeContainerListeners.remove(listener); }



private:
	uint32 lastNodeId;
	int32 lastConnectionId;

	JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(NodeContainer)
};


#endif  // NODECONTAINER_H_INCLUDED
