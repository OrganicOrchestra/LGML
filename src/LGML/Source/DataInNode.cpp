/*
  ==============================================================================

    DataInNode.cpp
    Created: 19 Apr 2016 10:38:42am
    Author:  bkupe

  ==============================================================================
*/

#include "DataInNode.h"
#include "DataInNodeUI.h"


DataInNode::DataInNode() :
    NodeBase("DataIn",NodeType::DataInType)
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

ConnectableNodeUI * DataInNode::createUI()
{
    return new NodeBaseUI(this,new DataInNodeContentUI);
}
