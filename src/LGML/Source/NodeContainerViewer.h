/*
  ==============================================================================

    NodeContainerViewer.h
    Created: 18 May 2016 8:22:16pm
    Author:  bkupe

  ==============================================================================
*/

#ifndef NODECONTAINERVIEWER_H_INCLUDED
#define NODECONTAINERVIEWER_H_INCLUDED

#include "NodeContainer.h"
#include "NodeBaseUI.h"

class NodeConnectionUI;

class NodeContainerViewer :
	public Component,
	public NodeContainerListener
{
public :
	NodeContainerViewer(NodeContainer * container);
	virtual ~NodeContainerViewer();

	NodeContainer * nodeContainer;

	OwnedArray<NodeBaseUI> nodesUI;
	OwnedArray<NodeConnectionUI>  connectionsUI;
	NodeConnectionUI * editingConnection;

	void clear();

	void resized() override;

	// Inherited via NodeContainerListener
	virtual void nodeAdded(ConnectableNode *) override;
	virtual void nodeRemoved(ConnectableNode *) override;
	virtual void connectionAdded(NodeConnection *) override;
	virtual void connectionRemoved(NodeConnection *) override;

	void addNodeUI(NodeBase * node);
	void removeNodeUI(NodeBase * node);
	NodeBaseUI * getUIForNode(NodeBase * node);

	void addConnectionUI(NodeConnection * connection);
	void removeConnectionUI(NodeConnection * connection);
	NodeConnectionUI * getUIForConnection(NodeConnection * connection);

	//connection creation / editing
	typedef ConnectorComponent Connector;
	void createDataConnectionFromConnector(Connector * baseConnector);
	void createAudioConnectionFromConnector(Connector * baseConnector);

	void updateEditingConnection();
	bool isEditingConnection() { return editingConnection != nullptr; }
	bool checkDropCandidates();
	bool setCandidateDropConnector(Connector * c);
	void cancelCandidateDropConnector();
	void finishEditingConnection();


	//Mouse event
	void mouseDown(const MouseEvent& event) override;
	void mouseMove(const MouseEvent& event) override;
	void mouseDrag(const MouseEvent& event) override;
	void mouseUp(const MouseEvent& event) override;
	void childBoundsChanged(Component *)override;

	void resizeToFitNodes();
};


#endif  // NODECONTAINERVIEWER_H_INCLUDED
