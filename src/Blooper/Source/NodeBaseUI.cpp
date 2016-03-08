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

#include "BoolToggleUI.h"

//==============================================================================
NodeBaseUI::NodeBaseUI(NodeBase * node) :
	inputContainer(ConnectorComponent::ConnectorIOType::INPUT),
	outputContainer(ConnectorComponent::ConnectorIOType::OUTPUT),
    node(node)
{

	DBG("Node Base UI Constructor");
	
	connectorWidth = 10;
	addAndMakeVisible(mainContainer);
	addAndMakeVisible(inputContainer);
	addAndMakeVisible(outputContainer);
	
	getHeaderContainer()->addMouseListener(this,false);// (true, true);
	
	setNode(node);

	setSize(300, 200);
	
}

NodeBaseUI::~NodeBaseUI()
{
    if(node && node->hasAudioOutputs){
        node->audioProcessor->removeListener(&vuMeter);
    }
	//this->node = nullptr;
}


void NodeBaseUI::setNode(NodeBase * node)
{
	this->node = node;
	mainContainer.setUIFromNode(node);

	inputContainer.setConnectorsFromNode(node);
	outputContainer.setConnectorsFromNode(node);

    if(node!=nullptr && node->hasAudioOutputs){
        node->audioProcessor->addListener(&vuMeter);
        getHeaderContainer()->addAndMakeVisible(vuMeter);
    }

	//parameters

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

    if(node!=nullptr && node->hasAudioOutputs){
		
		Rectangle<int> vuMeterRect = getHeaderContainer()->getLocalBounds().removeFromRight(14).reduced(4);
		vuMeter.setBounds(vuMeterRect);
    }

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

	c->setTopLeftPosition(0, 10 + getNumChildComponents()*(getHeight() + 30));

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
