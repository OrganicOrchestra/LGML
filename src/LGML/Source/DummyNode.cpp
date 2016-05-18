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

DummyNode::DummyNode(uint32 nodeId) :
    NodeBase(nodeId, "DummyNode")
{


    freq1Param = addFloatParameter("Freq 1", "This is a test int slider",.23f);
    freq2Param = addFloatParameter("Freq 2", "This is a test int slider", .55f);

    testTrigger = addTrigger("Test Trigger", "Youpi");

	//AUDIO
	setPlayConfigDetails(2, 3, getSampleRate(), getBlockSize());

	//DATA
	addInputData("IN Number", DataType::Number);
	addInputData("IN Position", DataType::Position);

	addOutputData("OUT Number", DataType::Number);
	addOutputData("OUT Orientation", DataType::Orientation);
}

 DummyNode::~DummyNode()
{
}

 void DummyNode::onContainerParameterChanged(Parameter * p)
 {
     if (p == freq1Param)
     {
         //       ((DummyAudioProcessor*)audioProcessor)->amp = p->getNormalizedValue();
         period1 = (int)(44100.0f / (1.0f + 440.0f*p->getNormalizedValue()));
     }
     else if (p == freq2Param)
     {
         period2 = (int)(44100.0f / (1.0f + 440.0f*p->getNormalizedValue()));
     }
 }

 NodeBaseUI * DummyNode::createUI()
{

    NodeBaseUI * ui = new NodeBaseUI(this,new DummyNodeContentUI());
    return ui;

}
