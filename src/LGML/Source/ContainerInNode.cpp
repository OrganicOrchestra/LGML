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
	NodeBase("Container IN", NodeType::ContainerInType)
{
	canBeRemovedByUser = false;
	canHavePresets = false;
	userCanAccessInputs = false;
	hasMainAudioControl = false;
}

ContainerInNode::~ContainerInNode()
{
}

void ContainerInNode::setNumAudioChannels(int channels)
{
	setPreferedNumAudioInput(channels);
	setPreferedNumAudioOutput(channels);
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
