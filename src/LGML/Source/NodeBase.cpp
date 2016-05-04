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
ControllableContainer(name),
nodeTypeUID(0) // UNKNOWNTYPE
{

    if (dataProcessor != nullptr)
    {
        dataProcessor->addDataProcessorListener(this);
    }

    checkInputsAndOutputs();
    addToAudioGraphIfNeeded();

    //set Params
    nameParam = addStringParameter("Name", "Set the name of the node.", name);
    enabledParam = addBoolParameter("Enabled", "Set whether the node is enabled or disabled", true);
    xPosition = addFloatParameter("xPosition","x position on canvas",0,0,99999);
    yPosition= addFloatParameter("yPosition","y position on canvas",0,0,99999);

    xPosition->isControllableExposed = false;
    yPosition->isControllableExposed = false;
    xPosition->isPresettable = false;
    yPosition->isPresettable = false;
    nameParam->isPresettable = false;
    enabledParam->isPresettable = false;
}


NodeBase::~NodeBase()
{

    removeFromAudioGraphIfNeeded();

    // get called after deletion of TimeManager on app exit
    TimeManager * tm = TimeManager::getInstanceWithoutCreating();
    if(tm!=nullptr)
        tm->releaseMasterNode(this);

}

void NodeBase::checkInputsAndOutputs()
{
    hasDataInputs = dataProcessor != nullptr ? dataProcessor->getTotalNumInputData()>0:false;
    hasDataOutputs = dataProcessor != nullptr ? dataProcessor->getTotalNumOutputData()>0:false;

    hasAudioInputs = audioProcessor != nullptr ? audioProcessor->getTotalNumInputChannels() > 0:false;
    hasAudioOutputs = audioProcessor != nullptr ? audioProcessor->getTotalNumOutputChannels() > 0:false;
}

void NodeBase::remove(bool askBeforeRemove)
{
    if (askBeforeRemove)
    {
        int result = AlertWindow::showOkCancelBox(AlertWindow::AlertIconType::QuestionIcon, "Remove node", "Do you want to remove the node ?");
        if (result == 0) return;
    }

    nodeListeners.call(&NodeBase::NodeListener::askForRemoveNode,this);
}

void NodeBase::inputAdded(Data *)
{
    hasDataInputs = dataProcessor != nullptr ? dataProcessor->getTotalNumInputData()>0:false;
}

void NodeBase::inputRemoved(Data *)
{
    hasDataInputs = dataProcessor != nullptr ? dataProcessor->getTotalNumInputData()>0:false;
}

void NodeBase::outputAdded(Data *)
{
    hasDataOutputs = dataProcessor != nullptr ? dataProcessor->getTotalNumOutputData()>0:false;
}

void NodeBase::ouputRemoved(Data *)
{
    hasDataOutputs = dataProcessor != nullptr ? dataProcessor->getTotalNumOutputData()>0:false;
}


void NodeBase::parameterValueChanged(Parameter * p)
{

    if (p == nameParam)
    {
        setNiceName(nameParam->stringValue());
    }else if (p == enabledParam)
    {
        if(audioProcessor){
            audioProcessor->suspendProcessing(!enabledParam->boolValue());
        }
        if(dataProcessor){
            dataProcessor->enabled = enabledParam->boolValue();
        }
    }
    else{
            ControllableContainer::parameterValueChanged(p);
    }
}

void NodeBase::addToAudioGraphIfNeeded(){
    if(hasAudioInputs || hasAudioOutputs){
        nodeManager->audioGraph.addNode(audioProcessor,nodeId);
        audioProcessor->addNodeAudioProcessorListener(this);
        addChildControllableContainer( audioProcessor);
    }
}
void NodeBase::removeFromAudioGraphIfNeeded(){
    if(hasAudioInputs || hasAudioOutputs){
        nodeManager->audioGraph.removeNode(nodeId);
        audioProcessor->removeNodeAudioProcessorListener(this);

		//@martin commented out because access violation error, is it needed ? (no leak detector when commented)
        //removeChildControllableContainer( audioProcessor);
    }
}

String NodeBase::getPresetFilter()
{
    return NodeFactory::nodeToString(this);
}


//Save / Load

var NodeBase::getJSONData()
{
	var data = ControllableContainer::getJSONData();
    data.getDynamicObject()->setProperty("nodeType", NodeFactory::nodeToString(this));
    data.getDynamicObject()->setProperty("nodeId", String(nodeId));

    if (audioProcessor) {
        MemoryBlock m;

        // TODO we could implement that for all node objects to be able to save any kind of custom data
        audioProcessor->getStateInformation(m);

        if (m.getSize()) {
            var audioProcessorData(new DynamicObject());
            audioProcessorData.getDynamicObject()->setProperty("state", m.toBase64Encoding());
            data.getDynamicObject()->setProperty("audioProcessor", audioProcessorData);
        }
    }

    return data;
}

void NodeBase::loadJSONDataInternal(var data)
{
    if (audioProcessor) {
        var audioProcessorData = data.getProperty("audioProcessor", var());
        String audioProcessorStateData = audioProcessorData.getProperty("state",var());

        MemoryBlock m;
        m.fromBase64Encoding(audioProcessorStateData);
        audioProcessor->setStateInformation(m.getData(), (int)m.getSize());
    }
}


void NodeBase::numAudioInputChanged(int ){

    nodeManager->audioGraph.prepareToPlay(nodeManager->audioGraph.getBlockSize(),(int)nodeManager->audioGraph.getSampleRate());
}
void NodeBase::numAudioOutputChanged(int ){
    nodeManager->audioGraph.prepareToPlay(nodeManager->audioGraph.getBlockSize(),(int)nodeManager->audioGraph.getSampleRate());
}
