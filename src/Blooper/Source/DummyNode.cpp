/*
  ==============================================================================

    DummyNode.cpp
    Created: 3 Mar 2016 12:31:33pm
    Author:  bkupe

  ==============================================================================
*/

#include "DummyNode.h"
#include "NodeBaseUI.h"


DummyNode::DummyNode(NodeManager * nodeManager,uint32 nodeId) :
	NodeBase(nodeManager,nodeId, "DummyNode", new DummyAudioProcessor, new DummyDataProcessor)
{

	DBG("Dummy node add Data Input");
	dataProcessor->addInputData("IN Number", DataProcessor::DataType::Number);
	dataProcessor->addInputData("IN Position", DataProcessor::DataType::Position);

	dataProcessor->addOutputData("OUT Number", DataProcessor::DataType::Number);
	dataProcessor->addOutputData("OUT Orientation", DataProcessor::DataType::Orientation);
}

 DummyNode::~DummyNode()
{
	DBG("delete dummy node");
}

NodeBaseUI * DummyNode::createUI()
{

	NodeBaseUI * ui = new NodeBaseUI(this);
	return ui;
	
}
