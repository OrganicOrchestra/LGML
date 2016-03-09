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

#include "NodeBaseHeaderUI.h"
#include "NodeBaseContentUI.h"

//==============================================================================
NodeBaseUI::NodeBaseUI(NodeBase * node, NodeBaseContentUI * contentContainer, NodeBaseHeaderUI * headerContainer) :
	inputContainer(ConnectorComponent::ConnectorIOType::INPUT),
	outputContainer(ConnectorComponent::ConnectorIOType::OUTPUT),
	mainContainer(contentContainer,headerContainer),
    node(node)
{

	DBG("Node Base UI Constructor");
	
	this->node = node;

	connectorWidth = 10;
	

	
	inputContainer.setConnectorsFromNode(node);
	outputContainer.setConnectorsFromNode(node);

	addAndMakeVisible(mainContainer);
	addAndMakeVisible(inputContainer);
	addAndMakeVisible(outputContainer);
	getHeaderContainer()->addMouseListener(this, false);// (true, true);
	
	mainContainer.setNodeAndNodeUI(node, this);
	if(getWidth() == 0 || getHeight() == 0) setSize(300, 200);
	
}

NodeBaseUI::~NodeBaseUI()
{
}


void NodeBaseUI::setNode(NodeBase * node)
{
	

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

NodeBaseUI::MainContainer::MainContainer(NodeBaseContentUI * content, NodeBaseHeaderUI * header) :
	ContourComponent(Colours::green),
	headerContainer(header), contentContainer(content)
{

	if (headerContainer == nullptr) headerContainer = new NodeBaseHeaderUI();
	if (contentContainer == nullptr) contentContainer = new NodeBaseContentUI();


	addAndMakeVisible(headerContainer);
	addAndMakeVisible(contentContainer);
}

void NodeBaseUI::MainContainer::setNodeAndNodeUI(NodeBase * node, NodeBaseUI * nodeUI)
{
	headerContainer->setNodeAndNodeUI(node, nodeUI);
	contentContainer->setNodeAndNodeUI(node, nodeUI);
}

void NodeBaseUI::MainContainer::paint(Graphics & g)
{
	g.setColour(PANEL_COLOR);
	g.fillRoundedRectangle(getLocalBounds().toFloat(), 4);
	g.setColour(CONTOUR_COLOR);
	g.drawRoundedRectangle(getLocalBounds().toFloat(), 4, 2);

}

void NodeBaseUI::MainContainer::resized()
{
	Rectangle<int> r = getLocalBounds();
	Rectangle<int> headerBounds = r.removeFromTop(headerContainer->getHeight());
	headerContainer->setBounds(headerBounds);
	contentContainer->setBounds(r);
}
