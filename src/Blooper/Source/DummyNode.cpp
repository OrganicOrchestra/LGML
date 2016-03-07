/*
  ==============================================================================

    DummyNode.cpp
    Created: 3 Mar 2016 12:31:33pm
    Author:  bkupe

  ==============================================================================
*/

#include "DummyNode.h"
#include "DummyNodeUI.h"


DummyNode::DummyNode(uint32 nodeId) :
	NodeBase(nodeId, "DummyNode", new DummyAudioProcessor, new DummyDataProcessor)
{

	DBG("Dummy node add Data Input");
	
	dataProcessor->addInputData("Dummy IN Number", DataProcessor::DataType::Number);
	dataProcessor->addInputData("Dummy IN Position", DataProcessor::DataType::Position);

	dataProcessor->addOutputData("Dummy OUT Number", DataProcessor::DataType::Number);
	dataProcessor->addOutputData("Dummy OUT Orientatin", DataProcessor::DataType::Orientation);
}

 DummyNode::~DummyNode()
{
	DBG("delete");
}

NodeBaseUI * DummyNode::createUI()
{

	DummyNodeUI * ui = new DummyNodeUI();
	ui->setNode(this);
	return ui;
	
}
