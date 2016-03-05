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
	
	dataProcessor->addInputData("Dummy Number", DataProcessor::DataType::Number);
	dataProcessor->addInputData("Dummy Position", DataProcessor::DataType::Position);
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
