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
	String editingDataName;
	String editingElementName;
	DataProcessor::DataType editingDataType;

	uint32 editingChannel;

	void clear();

    void paint (Graphics&);
    void resized();

	// Inherited via Listener
	virtual void nodeAdded(NodeBase *) override;
	virtual void nodeRemoved(NodeBase *) override;
	virtual void connectionAdded(NodeConnection *) override;
	virtual void connectionEdited(NodeConnection * ) override;
	virtual void connectionRemoved(NodeConnection *) override;


	void addNodeUI(NodeBase * node);
	void removeNodeUI(NodeBase * node);
	NodeBaseUI * getUIForNode(NodeBase * node);

	
	void addConnectionUI(NodeConnection * connection);
	void removeConnectionUI(NodeConnection * connection);
	NodeConnectionUI * getUIForConnection(NodeConnection * connection);

	//connection creation / editing
	typedef ConnectorComponent Connector;
	void createDataConnectionFromConnector(Connector * baseConnector, const String &dataName, const String &elementName, DataProcessor::DataType editingDataType = DataProcessor::DataType::Unknown);
	void createAudioConnectionFromConnector(Connector * baseConnector, uint32 channel);

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

private:
    
	JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (NodeManagerUI)

};


#endif  // NODEMANAGERUI_H_INCLUDED
