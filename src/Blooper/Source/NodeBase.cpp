/*
  ==============================================================================

    NodeBase.cpp
    Created: 2 Mar 2016 8:36:17pm
    Author:  bkupe

  ==============================================================================
*/

#include "NodeBase.h"
#include "NodeManager.h"
#include "TimeManager.h"


NodeBase::NodeBase(NodeManager * nodeManager,uint32 _nodeId, const String &name, NodeAudioProcessor * _audioProcessor, NodeDataProcessor * _dataProcessor) :
    nodeManager(nodeManager),
	nodeId(_nodeId),
	audioProcessor(_audioProcessor),
	dataProcessor(_dataProcessor),

	ControllableContainer(name)
{

	if (dataProcessor != nullptr)
	{
		dataProcessor->addListener(this);
		nodeManager->dataGraph.addNode(dataProcessor);
	}

	DBG("Node Base check inputs and outputs");
	checkInputsAndOutputs();
    addToAudioGraphIfNeeded();

	//set Params
	nameParam = addStringParameter("Name", "Set the name of the node.", name);
	nameParam->addListener(this);
	enabledParam = addBoolParameter("Enabled", "Set whether the node is enabled or disabled", true);
	enabledParam->addListener(this);
}


NodeBase::~NodeBase()
{
	DBG("delete NodeBase");
	removeFromAudioGraphIfNeeded();
    TimeManager::getInstance()->removeIfMaster(this);
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


void NodeBase::parameterValueChanged(Parameter * p)
{
	DBG("param value changed in nodeBase :" + p->niceName);
	if (p == nameParam) setNiceName(nameParam->value);
	else if (p == enabledParam)
	{
		DBG("set Node Enabled " + String(enabledParam->value));
	}
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