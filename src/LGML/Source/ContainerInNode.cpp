/*
  ==============================================================================

    ContainerInNode.cpp
    Created: 19 May 2016 5:44:32pm
    Author:  bkupe

  ==============================================================================
*/

#include "ContainerInNode.h"
#include "NodeBaseUI.h"

ContainerInNode::ContainerInNode() :
	NodeBase("Container IN", NodeType::UNKNOWN_TYPE)
{
	canBeRemovedByUser = false;
	canHavePresets = false;
}

ContainerInNode::~ContainerInNode()
{
}

void ContainerInNode::processInputDataChanged(Data * d)
{
	Data * od = getOutputDataByName(d->name);
	od->updateFromSourceData(d);
}

ConnectableNodeUI * ContainerInNode::createUI()
{
	return new NodeBaseUI(this);
}
