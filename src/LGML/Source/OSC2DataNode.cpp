/*
  ==============================================================================

    OSC2DataNode.cpp
    Created: 19 Apr 2016 10:38:42am
    Author:  bkupe

  ==============================================================================
*/

#include "OSC2DataNode.h"
#include "OSC2DataNodeUI.h"



OSC2DataNode::OSC2DataNode(NodeManager * nodeManager, uint32 nodeId) :
	NodeBase(nodeManager, nodeId, "OSC2Data", nullptr, new OSC2DataNodeDataProcessor)
{
	addFloatParamAndData("Test Param", 0, 0, 1);
}

OSC2DataNode::~OSC2DataNode()
{

}

void OSC2DataNode::parameterValueChanged(Parameter * p)
{
	dataProcessor->updateOutputData(p->niceName, p->getNormalizedValue());
}

NodeBaseUI * OSC2DataNode::createUI()
{
	return new NodeBaseUI(this,new OSC2DataNodeContentUI);
}

