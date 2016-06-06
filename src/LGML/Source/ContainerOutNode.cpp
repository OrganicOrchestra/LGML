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

	numInputChannels = addIntParameter("Num Audio Outputs", "Number of output channels for this container", 2, 0, 100);
	numInputData = addIntParameter("Num Data Outputs", "Number of data outputs for this container", 0, 0, 100);
}

ContainerOutNode::~ContainerOutNode()
{
	if (parentNodeContainer != nullptr)
	{
		removeRMSListener(parentNodeContainer);
	}
}



void ContainerOutNode::setNumAudioChannels(int channels)
{
	setPreferedNumAudioInput(channels);
	setPreferedNumAudioOutput(channels);
}

void ContainerOutNode::setParentNodeContainer(NodeContainer * nc)
{
	if (parentNodeContainer != nullptr)
	{
		removeRMSListener(parentNodeContainer);
	}
	NodeBase::setParentNodeContainer(nc);

	if (parentNodeContainer != nullptr)
	{
		addRMSListener(parentNodeContainer);
	}
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

void ContainerOutNode::onContainerParameterChanged(Parameter * p)
{
	NodeBase::onContainerParameterChanged(p);

	if (p == numInputChannels)
	{
		setNumAudioChannels(p->intValue());
	}
	else if (p == numInputData)
	{
		if (p->intValue() < getTotalNumOutputData())
		{
			removeOutputData(outputDatas[outputDatas.size() - 1]->name);
		}
		else
		{
			addOutputData("Output Data " + String(outputDatas.size()), DataType::Number);
		}
	}
}
