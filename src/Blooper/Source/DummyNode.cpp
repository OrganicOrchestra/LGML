/*
  ==============================================================================

    DummyNode.cpp
    Created: 3 Mar 2016 12:31:33pm
    Author:  bkupe

  ==============================================================================
*/

#include "DummyNode.h"
#include "NodeBaseUI.h"
#include "DummyNodeContentUI.h"

DummyNode::DummyNode(NodeManager * nodeManager,uint32 nodeId) :
	NodeBase(nodeManager,nodeId, "DummyNode", new DummyAudioProcessor, new DummyDataProcessor)
{

	dataProcessor->addInputData("IN Number", DataProcessor::DataType::Number);
	dataProcessor->addInputData("IN Position", DataProcessor::DataType::Position);

	dataProcessor->addOutputData("OUT Number", DataProcessor::DataType::Number);
	dataProcessor->addOutputData("OUT Orientation", DataProcessor::DataType::Orientation);

	testFloatParam = addFloatParameter("Test Int slider", "This is a test int slider",.23f);
	testFloatParam->addListener(this);
}

 DummyNode::~DummyNode()
{
}

 void DummyNode::parameterValueChanged(Parameter * p)
 {
	 NodeBase::parameterValueChanged(p);

	 if (p == testFloatParam)
	 {
		 DBG(testFloatParam->value);
		 ((DummyAudioProcessor*)audioProcessor)->amp = p->getNormalizedValue();
	 }
 }

 NodeBaseUI * DummyNode::createUI()
{

	NodeBaseUI * ui = new NodeBaseUI(this,new DummyNodeContentUI());
	return ui;
	
}
