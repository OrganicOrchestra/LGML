/*
  ==============================================================================

    ContainerInNode.cpp
    Created: 19 May 2016 5:44:32pm
    Author:  bkupe

  ==============================================================================
*/

#include "ContainerInNode.h"
#include "NodeBaseUI.h"
#include "NodeContainer.h"

ContainerInNode::ContainerInNode() :
	NodeBase("Container IN", NodeType::ContainerInType, false),
AudioGraphIOProcessor(AudioProcessorGraph::AudioGraphIOProcessor::IODeviceType::audioInputNode)
{
	canBeRemovedByUser = false;
	canHavePresets = false;
	userCanAccessInputs = false;

	numInputChannels = addIntParameter("Num Audio Inputs", "Number of input channels for this container", 2, 0, 100);
	numInputData = addIntParameter("Num Data Inputs", "Number of data inputs for this container", 0, 0, 100);
}

ContainerInNode::~ContainerInNode()
{
	if (parentNodeContainer != nullptr)
	{
		removeRMSListener(parentNodeContainer);
	}
}

void ContainerInNode::processBlockInternal(AudioBuffer<float>& buffer, MidiBuffer & midiMessages) {
  AudioProcessorGraph::AudioGraphIOProcessor::processBlock(buffer, midiMessages);

};

void ContainerInNode::setParentNodeContainer(NodeContainer * nc)
{
	NodeBase::setParentNodeContainer(nc);
  setPreferedNumAudioInput(0);
  setPreferedNumAudioOutput(nc->getTotalNumInputChannels());
}


void ContainerInNode::setNumAudioChannels(int channels)
{

  // only handle one container in per container for now
  parentNodeContainer->setPreferedNumAudioInput(channels);
  {
//  parentNodeContainer->suspendProcessing(true);
	setPreferedNumAudioOutput(channels);
//    parentNodeContainer->suspendProcessing(false);
  }
  jassert(parentNodeContainer);

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

void ContainerInNode::onContainerParameterChanged(Parameter * p)
{
	NodeBase::onContainerParameterChanged(p);

	if (p == numInputChannels)
	{
		setNumAudioChannels(p->intValue());
	}
	else if (p == numInputData)
	{
		if (p->intValue() < getTotalNumInputData())
		{
			removeInputData(inputDatas[inputDatas.size() - 1]->name);
		}
		else
		{
			addInputData("Input Data " + String(inputDatas.size()),DataType::Number);
		}
	}

}
