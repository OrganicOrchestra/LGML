/*
  ==============================================================================

    NodeBase.cpp
    Created: 2 Mar 2016 8:36:17pm
    Author:  bkupe

  ==============================================================================
*/

#include "NodeBase.h"
#include "NodeManager.h"

NodeBase::NodeBase(NodeManager * nodeManager,uint32 _nodeId, String name, NodeAudioProcessor * _audioProcessor, NodeDataProcessor * _dataProcessor) :
    nodeManager(nodeManager),
	nodeId(_nodeId),
	audioProcessor(_audioProcessor),
	dataProcessor(_dataProcessor),
	name(name)
{

	if(dataProcessor != nullptr) dataProcessor->addListener(this);
	DBG("Node Base check inputs and outputs");
	checkInputsAndOutputs();
    addToAudioGraphIfNeeded();
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
    removeFromAudioGraphIfNeeded();
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

void NodeBase::addToAudioGraphIfNeeded(){
    if(hasAudioInputs || hasAudioOutputs){
        nodeManager->audioGraph.addNode(audioProcessor,nodeId);
    }
}
void NodeBase::removeFromAudioGraphIfNeeded(){
    if(hasAudioInputs || hasAudioOutputs){
        nodeManager->audioGraph.removeNode(nodeId);
    }
}

void NodeBase::NodeDataProcessor::receiveData(const Data * incomingData, const String &destDataName, const String &destElementName, const String &sourceElementName)
{
}

void NodeBase::NodeDataProcessor::sendData(const Data * outgoingData, const String &sourceElementName)
{
	
}