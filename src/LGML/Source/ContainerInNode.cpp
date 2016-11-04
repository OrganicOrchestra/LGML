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

	numChannels = addIntParameter("Num Audio Inputs", "Number of input channels for this container", 2, 0, 100);
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
  // graphs can be fed with bigger amount of channel (if numoutputChannel>numInputChannel)
  // we need to clear them
  for(int i = totalNumOutputChannels;i < buffer.getNumChannels() ; i++){
    buffer.clear(i,0,buffer.getNumSamples());
  }

};

void ContainerInNode::setParentNodeContainer(NodeContainer * nc)
{
	NodeBase::setParentNodeContainer(nc);
  setPreferedNumAudioInput(0);
  setPreferedNumAudioOutput(nc->getTotalNumInputChannels());
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

void ContainerInNode::setNumChannels(int num){
  setPreferedNumAudioOutput(num);
  if(parentNodeContainer){parentNodeContainer->setPreferedNumAudioInput(totalNumOutputChannels);}
}

void ContainerInNode::onContainerParameterChanged(Parameter * p)
{
	NodeBase::onContainerParameterChanged(p);

	if (p == numChannels)
	{
    setNumChannels(p->intValue());
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
