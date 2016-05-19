/*
  ==============================================================================

    ConnectableNodeUI.h
    Created: 18 May 2016 11:34:23pm
    Author:  bkupe

  ==============================================================================
*/

#ifndef CONNECTABLENODEUI_H_INCLUDED
#define CONNECTABLENODEUI_H_INCLUDED

#include "InspectableComponent.h"
#include "ConnectableNode.h"
#include "ConnectorComponent.h"

class ConnectableNodeUI : public InspectableComponent
{
public:
	ConnectableNodeUI(ConnectableNode * cn);
	virtual ~ConnectableNodeUI();

	//layout
	int connectorWidth;

	class ConnectorContainer : public Component
	{
	public:
		OwnedArray<ConnectorComponent> connectors;

		ConnectorComponent::ConnectorDisplayLevel displayLevel;
		ConnectorComponent::ConnectorIOType type;

		ConnectorContainer(ConnectorComponent::ConnectorIOType type);



		void setConnectorsFromNode(NodeBase * node);
		void addConnector(ConnectorComponent::ConnectorIOType ioType, NodeConnection::ConnectionType dataType, NodeBase * node);
		void resized();

		ConnectorComponent * getFirstConnector(NodeConnection::ConnectionType dataType);


		
		JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(ConnectorContainer)
	};

	ConnectorContainer inputContainer;
	ConnectorContainer outputContainer;


	Array<ConnectorComponent *> getComplementaryConnectors(ConnectorComponent * baseConnector); 

	//Need to clean out and decide whether there can be more than 1 data connector / audio connector on nodes
	ConnectorComponent * getFirstConnector(NodeConnection::ConnectionType connectionType, ConnectorComponent::ConnectorIOType ioType);

	 
	ConnectableNode * connectableNode;
};


#endif  // CONNECTABLENODEUI_H_INCLUDED
