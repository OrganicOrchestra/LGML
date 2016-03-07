/*
  ==============================================================================

    NodeBaseUI.cpp
    Created: 3 Mar 2016 11:52:50pm
    Author:  bkupe

  ==============================================================================
*/

#include "../JuceLibraryCode/JuceHeader.h"
#include "NodeBaseUI.h"
#include "NodeBase.h"
#include "NodeManagerUI.h"
#include "ConnectorComponent.h"

//==============================================================================
NodeBaseUI::NodeBaseUI() :
	inputContainer(ConnectorComponent::ConnectorIOType::INPUT),
	outputContainer(ConnectorComponent::ConnectorIOType::OUTPUT)
{

	DBG("Node Base UI Constructor");
	
	connectorWidth = 10;

	addAndMakeVisible(mainContainer);
	addAndMakeVisible(inputContainer);
	addAndMakeVisible(outputContainer);
	
	setSize(300,200);

	
	getHeaderContainer()->addMouseListener(this,true);// (true, true);
	
}

NodeBaseUI::~NodeBaseUI()
{
	this->node = nullptr;
}


void NodeBaseUI::setNode(NodeBase * node)
{
	this->node = node;
	mainContainer.titleLabel.setText(node->name,NotificationType::sendNotification);
	inputContainer.setConnectorsFromNode(node);
	outputContainer.setConnectorsFromNode(node);
}


void NodeBaseUI::paint (Graphics& g)
{

}

void NodeBaseUI::resized()
{
	Rectangle<int> r = getLocalBounds();
	Rectangle<int> inputBounds = r.removeFromLeft(connectorWidth);
	Rectangle<int> outputBounds = r.removeFromRight(connectorWidth);

	mainContainer.setBounds(r);
	inputContainer.setBounds(inputBounds);
	outputContainer.setBounds(outputBounds);
}

NodeManagerUI * NodeBaseUI::getNodeManagerUI() const noexcept
{
	return findParentComponentOfClass<NodeManagerUI>();
}

void NodeBaseUI::mouseDown(const MouseEvent & e)
{
	if (e.mods.getCurrentModifiers().isCtrlDown())
	{
		DBG("Node->remove");
		node->remove();
	}
	else
	{
		nodeInitPos = getPosition();
	}
	
}

void NodeBaseUI::mouseDrag(const MouseEvent & e)
{
	Point<int> diff = Point<int>(e.getPosition() - e.getMouseDownPosition());
	setTopLeftPosition(nodeInitPos + diff);
}



// ======= CONNECTOR CONTAINER AND CONNECTOR COMPONENT ===================
NodeBaseUI::ConnectorContainer::ConnectorContainer(ConnectorComponent::ConnectorIOType type) :ContourComponent(), type(type), displayLevel(ConnectorComponent::MINIMAL)
{
}

void NodeBaseUI::ConnectorContainer::setConnectorsFromNode(NodeBase * node)
{
	connectors.clear();
	
	//for later : this is the creation for minimal display level
	bool hasAudio = (type == ConnectorComponent::INPUT) ? node->hasAudioInputs : node->hasAudioOutputs;
	bool hasData = (type == ConnectorComponent::INPUT) ? node->hasDataInputs : node->hasDataOutputs;

	if (hasAudio)
	{
		addConnector(type, NodeConnection::ConnectionType::AUDIO, node);
	}

	if (hasData)
	{
		DBG("Set connectors from node, connector is data, num inputs ?" + String(node->dataProcessor->getTotalNumInputData()));
		addConnector(type, NodeConnection::ConnectionType::DATA, node);
	}
}

void NodeBaseUI::ConnectorContainer::addConnector(ConnectorComponent::ConnectorIOType ioType, NodeConnection::ConnectionType dataType, NodeBase * node)
{
	ConnectorComponent * c = new ConnectorComponent(ioType, dataType, node);

	c->setTopLeftPosition(0, 20 + getNumChildComponents()*(getWidth() + 10));

	connectors.add(c);
	addAndMakeVisible(c);
}

void NodeBaseUI::ConnectorContainer::resized()
{
	for (int i = connectors.size() - 1; i >= 0; --i)
	{
		getChildComponent(i)->setSize(getWidth(), getWidth());
	}
}
