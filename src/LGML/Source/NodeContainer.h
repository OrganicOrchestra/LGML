/*
  ==============================================================================

    NodeContainer.h
    Created: 18 May 2016 7:53:56pm
    Author:  bkupe

  ==============================================================================
*/

#ifndef NODECONTAINER_H_INCLUDED
#define NODECONTAINER_H_INCLUDED


#include "NodeFactory.h"
#include "ConnectableNode.h"
#include "NodeConnection.h"

#include "ContainerInNode.h"
#include "ContainerOutNode.h"


//Listener
class  NodeContainerListener
{
public:
	/** Destructor. */
	virtual ~NodeContainerListener() {}

	virtual void nodeAdded(ConnectableNode *) = 0;
	virtual void nodeRemoved(ConnectableNode *) = 0;

	virtual void connectionAdded(NodeConnection *) = 0;
	virtual void connectionRemoved(NodeConnection *) = 0;
};


class NodeContainer : 
	public ConnectableNode,
	public ConnectableNode::ConnectableNodeListener, 
	public NodeConnection::Listener
{
public:
	NodeContainer(const String &name, NodeContainer * _parentNodeContainer = nullptr);
	virtual ~NodeContainer();


	//CONTAINER RELATED

	NodeContainer * parentNodeContainer;

	//Container nodes, not removable by user, handled separately
	ContainerInNode * containerInNode;
	ContainerOutNode * containerOutNode;


	//NODE AND CONNECTION MANAGEMENT

	Array<ConnectableNode *> nodes; //Not OwnedArray anymore because NodeBase is AudioProcessor, therefore owned by AudioProcessorGraph
	OwnedArray<NodeConnection> connections;

	ConnectableNode* addNode(NodeType nodeType);
	ConnectableNode* addNode(ConnectableNode * node);
	bool removeNode(ConnectableNode * n);

	ConnectableNode * getNodeForName(const String &name);

	NodeConnection * getConnection(const int index) const noexcept { return connections[index]; }
	NodeConnection * getConnectionBetweenNodes(ConnectableNode * sourceNode, ConnectableNode * destNode, NodeConnection::ConnectionType connectionType);
	Array<NodeConnection *> getAllConnectionsForNode(ConnectableNode * node);

	NodeConnection * addConnection(ConnectableNode * sourceNode, ConnectableNode * destNode, NodeConnection::ConnectionType connectionType);
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


	//AUDIO 
	AudioProcessorGraph::Node * getAudioNode(bool isInput) override;

	//DATA

private:
	JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(NodeContainer)
};


#endif  // NODECONTAINER_H_INCLUDED
