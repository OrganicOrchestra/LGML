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

    freq1Param = addFloatParameter("Freq 1", "This is a test int slider",.23f);
    freq2Param = addFloatParameter("Freq 2", "This is a test int slider", .55f);

    testTrigger = addTrigger("Test Trigger", "Youpi");
}

 DummyNode::~DummyNode()
{
}

 void DummyNode::onContainerParameterChanged(Parameter * p)
 {
     if (p == freq1Param)
     {
         //       ((DummyAudioProcessor*)audioProcessor)->amp = p->getNormalizedValue();
         ((DummyAudioProcessor*)audioProcessor)->period1 = (int)(44100.0f / (1.0f + 440.0f*p->getNormalizedValue()));
     }
     else if (p == freq2Param)
     {
         ((DummyAudioProcessor*)audioProcessor)->period2 = (int)(44100.0f / (1.0f + 440.0f*p->getNormalizedValue()));
     }
 }

 NodeBaseUI * DummyNode::createUI()
{

    NodeBaseUI * ui = new NodeBaseUI(this,new DummyNodeContentUI());
    return ui;

}
