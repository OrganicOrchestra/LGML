/*
  ==============================================================================

    DataInNode.cpp
    Created: 19 Apr 2016 10:38:42am
    Author:  bkupe

  ==============================================================================
*/

#include "DataInNode.h"
#include "DataInNodeUI.h"


DataInNode::DataInNode(NodeManager * nodeManager, uint32 nodeId) :
    NodeBase(nodeManager, nodeId, "DataIn")
{
    addFloatParamAndData("Test Param", 0, 0, 1);
}

DataInNode::~DataInNode()
{

}

void DataInNode::onContainerParameterChanged(Parameter * p)
{
  updateOutputData(p->niceName, p->getNormalizedValue());
}

NodeBaseUI * DataInNode::createUI()
{
    return new NodeBaseUI(this,new DataInNodeContentUI);
}
