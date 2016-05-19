/*
  ==============================================================================

    ContainerOutNode.cpp
    Created: 19 May 2016 5:44:22pm
    Author:  bkupe

  ==============================================================================
*/

#include "ContainerOutNode.h"

#include "NodeBaseUI.h"

ContainerOutNode::ContainerOutNode() :
	NodeBase("Container Out", NodeType::UNKNOWN_TYPE)
{
	canBeRemovedByUser = false;
	canHavePresets = false;
}

ContainerOutNode::~ContainerOutNode()
{
}

void ContainerOutNode::processInputDataChanged(Data * d)
{
	Data * od = getOutputDataByName(d->name);
	od->updateFromSourceData(d);
}

NodeBaseUI * ContainerOutNode::createUI()
{
	return new NodeBaseUI(this);
}
