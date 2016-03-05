/*
  ==============================================================================

    NodeManagerUI.h
    Created: 3 Mar 2016 10:38:22pm
    Author:  bkupe

  ==============================================================================
*/

#ifndef NODEMANAGERUI_H_INCLUDED
#define NODEMANAGERUI_H_INCLUDED


#include "NodeManager.h"
#include "NodeBaseUI.h"

class NodeConnectionUI;

//==============================================================================
/*
	Draw all connected Nodes and Connections
*/
class NodeManagerUI : public Component, public NodeManager::Listener
{
public:
	NodeManagerUI(NodeManager * nodeManager);
    ~NodeManagerUI();

	NodeManager * nodeManager;

	OwnedArray<NodeBaseUI> nodesUI;
	OwnedArray<NodeConnectionUI>  connectionsUI;
	NodeConnectionUI * editingConnection;
	
	void clear();

    void paint (Graphics&);
    void resized();

	// Inherited via Listener
	virtual void nodeAdded(NodeBase *) override;
	virtual void nodeRemoved(NodeBase *) override;
	virtual void audioConnectionAdded(AudioProcessorGraph::Connection *) override;
	virtual void audioConnectionRemoved(AudioProcessorGraph::Connection *) override;
	virtual void dataConnectionAdded(DataProcessorGraph::Connection *) override;
	virtual void dataConnectionRemoved(DataProcessorGraph::Connection *) override;


	void addNodeUI(NodeBase * node);
	void removeNodeUI(NodeBase * node);
	NodeBaseUI * getUIForNode(NodeBase * node);

	
	//connection creation / editing
	typedef NodeBaseUI::ConnectorContainer::ConnectorComponent Connector;
	void createConnectionFromConnector(Connector * baseConnector);
	void updateEditingConnection();
	bool isEditingConnection() { return editingConnection != nullptr; }
	bool checkDropCandidates();
	bool setCandidateDropConnector(Connector * c);
	void cancelCandidateDropConnector();
	void finishEditingConnection(Connector *c);

	void addAudioConnectionUI(AudioProcessorGraph::Connection * connection);
	void removeAudioConnectionUI(AudioProcessorGraph::Connection * connection);
	NodeConnectionUI *  getUIForAudioConnection(AudioProcessorGraph::Connection * connection);

	void addDataConnectionUI(DataProcessorGraph::Connection * connection);
	void removeDataConnectionUI(DataProcessorGraph::Connection * connection);
	NodeConnectionUI * getUIForDataConnection(DataProcessorGraph::Connection * connection);
	

	//Mouse event
	void mouseDown(const MouseEvent& event) override;

private:
    
	JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (NodeManagerUI)

};


#endif  // NODEMANAGERUI_H_INCLUDED
