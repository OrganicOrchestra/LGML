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
nodeTypeEnum(-1)
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
}


NodeBase::~NodeBase()
{

    removeFromAudioGraphIfNeeded();

    // get called after deletion of TimeManager on app exit
    TimeManager * tm = TimeManager::getInstanceWithoutCreating();
    if(tm!=nullptr)
        tm->removeIfMaster(this);

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
    ControllableContainer::parameterValueChanged(p);

    if (p == nameParam)
    {
        setNiceName(nameParam->value);
    }else if (p == enabledParam)
    {
        DBG("set Node Enabled " + String(enabledParam->value));
    }
}


void NodeBase::addToAudioGraphIfNeeded(){
    if(hasAudioInputs || hasAudioOutputs){
        nodeManager->audioGraph.addNode(audioProcessor,nodeId);
        audioProcessor->addNodeAudioProcessorListener(this);
    }
}
void NodeBase::removeFromAudioGraphIfNeeded(){
    if(hasAudioInputs || hasAudioOutputs){
        nodeManager->audioGraph.removeNode(nodeId);
        audioProcessor->removeNodeAudioProcessorListener(this);
    }
}


//Save / Load

var NodeBase::getJSONData()
{
    var data(new DynamicObject());
    data.getDynamicObject()->setProperty("nodeType", NodeFactory::nodeToString(this));
    data.getDynamicObject()->setProperty("nodeId", String(nodeId));

    var paramsData;

    Array<Controllable *> cont = ControllableContainer::getAllControllables(true, true);

    for (auto &c : cont) {
        Parameter * base = dynamic_cast<Parameter*>(c);
        if (base)
        {
            var pData(new DynamicObject());
            pData.getDynamicObject()->setProperty("controlAddress", base->getControlAddress(this));
            pData.getDynamicObject()->setProperty("value", base->toString());
            paramsData.append(pData);
        }
        else if (dynamic_cast<Trigger*>(c) != nullptr) {

        }
        else {
            // should never happen un less another Controllable type than parameter or trigger has been introduced
            jassertfalse;
        }
    }

    data.getDynamicObject()->setProperty("parameters", paramsData);

    //    for (int i = 0; i < PluginWindow::NumTypes; ++i)
    //    {
    //          .... do we need support of other windows than main PluginWindow?
    //    }

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

void NodeBase::loadJSONData(var data)
{

    //TODO : Move parameters save/load in ControllableContainer, so we only need to call ControllableContainer::loadJSONData() to handle parameters of a class inheriting CContainer;
    Array<var> * paramsData = data.getProperty("parameters",var()).getArray();

    for (var &pData : *paramsData)
    {
        String pControlAddress = pData.getProperty("controlAddress",var());// getProperymakeAddressFromXMLAttribute(paramXml->getAttributeName(i));

        Controllable * c = getControllableForAddress(pControlAddress, true, true);
        if (Parameter * p = dynamic_cast<Parameter*>(c)) {
            p->fromString(pData.getProperty("value",var())); //need to have a var-typed variable in parameter, so we can take advantage of autotyping
        }
        else {
            DBG("NodeBase::loadJSONData -> other Controllable than Parameters?");
            jassertfalse;
        }
    }

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
