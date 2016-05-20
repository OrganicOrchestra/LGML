/*
  ==============================================================================

    ContainerOutNode.cpp
    Created: 19 May 2016 5:44:22pm
    Author:  bkupe

  ==============================================================================
*/

#include "ContainerOutNode.h"
#include "NodeBaseUI.h"
#include "NodeContainer.h"


ContainerOutNode::ContainerOutNode() :
	NodeBase("Container Out", NodeType::ContainerOutType, false)
{
	canBeRemovedByUser = false;
	canHavePresets = false;
	userCanAccessOutputs = false;

	addOutputData("Data Kool", DataType::Position);
}

ContainerOutNode::~ContainerOutNode()
{
}


NodeContainer * ContainerOutNode::getParentNodeContainer()
{
	return (NodeContainer *)parentContainer;
}

void ContainerOutNode::setNumAudioChannels(int channels)
{
	setPreferedNumAudioInput(channels);
	setPreferedNumAudioOutput(channels);
}


void ContainerOutNode::processInputDataChanged(Data * d)
{
	Data * od = getOutputDataByName(d->name);
	od->updateFromSourceData(d);
}

ConnectableNodeUI * ContainerOutNode::createUI()
{
	return new NodeBaseUI(this);
}
