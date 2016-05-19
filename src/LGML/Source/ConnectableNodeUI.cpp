/*
  ==============================================================================

    ConnectableNodeUI.cpp
    Created: 18 May 2016 11:34:23pm
    Author:  bkupe

  ==============================================================================
*/

#include "ConnectableNodeUI.h"

ConnectableNodeUI::ConnectableNodeUI(ConnectableNode * cn) : 
	InspectableComponent(cn),
	connectableNode(cn),
	inputContainer(ConnectorComponent::ConnectorIOType::INPUT),
	outputContainer(ConnectorComponent::ConnectorIOType::OUTPUT)
 {

}

ConnectableNodeUI::~ConnectableNodeUI()
{
}



// ======= CONNECTOR CONTAINER AND CONNECTOR COMPONENT ===================
ConnectableNodeUI::ConnectorContainer::ConnectorContainer(ConnectorComponent::ConnectorIOType type) : type(type), displayLevel(ConnectorComponent::MINIMAL)
{
	setInterceptsMouseClicks(false, true);
}

void ConnectableNodeUI::ConnectorContainer::setConnectorsFromNode(NodeBase * _node)
{
	connectors.clear();

	//for later : this is the creation for minimal display level
	bool hasAudio = (type == ConnectorComponent::INPUT) ? _node->hasAudioInputs() : _node->hasAudioOutputs();
	bool hasData = (type == ConnectorComponent::INPUT) ? _node->hasDataInputs() : _node->hasDataOutputs();

	if (hasAudio)
	{
		addConnector(type, NodeConnection::ConnectionType::AUDIO, _node);
	}

	if (hasData)
	{
		addConnector(type, NodeConnection::ConnectionType::DATA, _node);
	}
}

void ConnectableNodeUI::ConnectorContainer::addConnector(ConnectorComponent::ConnectorIOType ioType, NodeConnection::ConnectionType dataType, NodeBase * _node)
{
	ConnectorComponent * c = new ConnectorComponent(ioType, dataType, _node);

	c->setTopLeftPosition(0, 10 + getNumChildComponents()*(getHeight() + 30));

	connectors.add(c);
	addAndMakeVisible(c);
}

void ConnectableNodeUI::ConnectorContainer::resized()
{
	for (int i = connectors.size() - 1; i >= 0; --i)
	{
		getChildComponent(i)->setSize(getWidth(), getWidth());
	}
}

ConnectorComponent * ConnectableNodeUI::ConnectorContainer::getFirstConnector(NodeConnection::ConnectionType dataType)
{
	for (int i = 0; i < connectors.size(); i++)
	{
		if (connectors.getUnchecked(i)->dataType == dataType) return connectors.getUnchecked(i);
	}

	return nullptr;
}



Array<ConnectorComponent*> ConnectableNodeUI::getComplementaryConnectors(ConnectorComponent * baseConnector)
{
	Array<ConnectorComponent *> result;


	ConnectorContainer * checkSameCont = baseConnector->ioType == ConnectorComponent::ConnectorIOType::INPUT ? &inputContainer : &outputContainer;
	if (checkSameCont->getIndexOfChildComponent(baseConnector) != -1) return result;

	ConnectorContainer * complCont = checkSameCont == &inputContainer ? &outputContainer : &inputContainer;
	for (int i = 0; i < complCont->connectors.size(); i++)
	{
		ConnectorComponent *c = (ConnectorComponent *)complCont->getChildComponent(i);
		if (c->dataType == baseConnector->dataType)
		{
			result.add(c);
		}
	}

	return result;
}

//Need to clean out and decide whether there can be more than 1 data connector / audio connector on nodes

ConnectorComponent * ConnectableNodeUI::getFirstConnector(NodeConnection::ConnectionType connectionType, ConnectorComponent::ConnectorIOType ioType)
{
	if (ioType == ConnectorComponent::INPUT)
	{
		return inputContainer.getFirstConnector(connectionType);
	}
	else
	{
		return outputContainer.getFirstConnector(connectionType);
	}
}