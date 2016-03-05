/*
  ==============================================================================

    NodeBase.cpp
    Created: 2 Mar 2016 8:36:17pm
    Author:  bkupe

  ==============================================================================
*/

#include "NodeBase.h"


NodeBase::NodeBase(uint32 _nodeId, String name, NodeAudioProcessor * _audioProcessor, NodeDataProcessor * _dataProcessor) :
	nodeId(_nodeId),
	audioProcessor(_audioProcessor),
	dataProcessor(_dataProcessor),
	name(name)
{

	dataProcessor->addListener(this);
	DBG("Node Base check inputs and outputs");
	checkInputsAndOutputs();
}

void NodeBase::checkInputsAndOutputs()
{
	hasDataInputs = dataProcessor != nullptr ? dataProcessor->getTotalNumInputData()>0:false;
	hasDataOutputs = dataProcessor != nullptr ? dataProcessor->getTotalNumOutputData()>0:false;

	hasAudioInputs = audioProcessor != nullptr ? audioProcessor->getTotalNumInputChannels() > 0:false;
	hasAudioOutputs = audioProcessor != nullptr ? audioProcessor->getTotalNumOutputChannels() > 0:false;
}

void NodeBase::remove()
{
	DBG("NodeBase::remove, disaptch askForRemove");
	listeners.call(&NodeBase::Listener::askForRemoveNode,this);
}

void NodeBase::inputAdded(DataProcessor::Data *)
{
	hasDataInputs = dataProcessor != nullptr ? dataProcessor->getTotalNumInputData()>0:false;
}

void NodeBase::inputRemoved(DataProcessor::Data *)
{
	hasDataInputs = dataProcessor != nullptr ? dataProcessor->getTotalNumInputData()>0:false;
}

void NodeBase::outputAdded(DataProcessor::Data *)
{
	hasDataOutputs = dataProcessor != nullptr ? dataProcessor->getTotalNumOutputData()>0:false;
}

void NodeBase::ouputRemoved(DataProcessor::Data *)
{
	hasDataOutputs = dataProcessor != nullptr ? dataProcessor->getTotalNumOutputData()>0:false;
}

NodeBase::~NodeBase()
{
	DBG("delete NodeBase");
	/*
	if (ui != nullptr)
	{
	delete ui;
	}
	*/
}

void NodeBase::NodeDataProcessor::receiveData(const Data * incomingData, String destDataName, String destElementName, String sourceElementName)
{
}

void NodeBase::NodeDataProcessor::sendData(const Data * outgoingData, String sourceElementName)
{
	
}