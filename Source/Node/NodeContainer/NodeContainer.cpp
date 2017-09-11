/* Copyright © Organic Orchestra, 2017
*
* This file is part of LGML.  LGML is a software to manipulate sound in realtime
*
* This program is free software; you can redistribute it and/or modify
* it under the terms of the GNU General Public License as published by
* the Free Software Foundation (version 3 of the License).
*
* This program is distributed in the hope that it will be useful,
* but WITHOUT ANY WARRANTY; without even the implied warranty of
* MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
*
*/


#include "NodeContainer.h"
#include "../Manager/NodeManager.h"
#include "../Connection/NodeConnection.h"
#include "UI/NodeContainerUI.h"


#include "../../Utils/DebugHelpers.h"



extern AudioDeviceManager& getAudioDeviceManager();
bool isEngineLoadingFile();

NodeContainer::NodeContainer(const String &name) :
containerInNode(nullptr),
containerOutNode(nullptr),
NodeBase(name, NodeType::ContainerType,false),
nodeChangeNotifier(10000),
rebuildTimer(this)
{

  innerGraph = new AudioProcessorGraph();
  innerGraph->releaseResources();
  setPreferedNumAudioOutput(2);
  setPreferedNumAudioInput(2);
#ifdef MULTITHREADED_AUDIO
  graphJob = new GraphJob(innerGraph,name);
  nodeManager = nullptr;
#endif

  //Force non recursive saving of preset as container has only is level to take care, nested containers are other nodes
  presetSavingIsRecursive = false;
}


NodeContainer::~NodeContainer()
{
  //connections.clear();
  rebuildTimer.stopTimer();
  clear(false);
  innerGraph->releaseResources();
}
void NodeContainer::clear(){

  //    innerGraph->clear();
  clear(false);


  innerGraph->releaseResources();

}
void NodeContainer::clear(bool recreateContainerNodes)
{

  while (connections.size() > 0)
  {
    connections[0]->remove();
  }



  while (nodes.size() > 0)
  {
    if(nodes[0].get()){
      nodes[0]->remove();
    }
    else{
      jassertfalse;
      nodes.remove(0);
    }


  }


  containerInNode = nullptr;
  containerOutNode = nullptr;
  setPreferedNumAudioOutput(2);
  setPreferedNumAudioInput(2);

  if (recreateContainerNodes && parentNodeContainer != nullptr)
  {
    containerInNode = (ContainerInNode *)addNode(new ContainerInNode());
    containerOutNode = (ContainerOutNode *)addNode(new ContainerOutNode());

    containerInNode->nodePosition->setPoint(150,100);
    containerOutNode->nodePosition->setPoint(450,100);

    //maybe keep it ?
    addConnection(containerInNode, containerOutNode, NodeConnection::ConnectionType::AUDIO);
  }

  if(!recreateContainerNodes)ConnectableNode::clear();

  // init with global sample rate and blockSize
  AudioIODevice * ad = getAudioDeviceManager().getCurrentAudioDevice();
  if(ad){
    setRateAndBufferSizeDetails(ad->getCurrentSampleRate(), ad->getCurrentBufferSizeSamples());
  }

}


ConnectableNode * NodeContainer::addNode(NodeType nodeType, const String &nodeName)
{
  ConnectableNode * n = NodeFactory::createNode(nodeType);
  return addNode(n,nodeName);
}

ConnectableNode * NodeContainer::addNodeFromJSONData(var data)
{
  NodeType nodeType = NodeFactory::getTypeFromString(data.getProperty("nodeType", var()));
  ConnectableNode * n = NodeFactory::createNode(nodeType);
  return addNode(n,n->getNiceName(),data);
}

ConnectableNode * NodeContainer::addNode(ConnectableNode * n, const String &nodeName, var nodeData)
{
  nodes.add(n);

  n->setParentNodeContainer(this);

  if (NodeContainer * nc = dynamic_cast<NodeContainer*>(n))
  {
    nodeContainers.add(nc);
    nc->NodeContainer::clear(!isEngineLoadingFile());
    //DBG("Check containerIn Node : " << String(((NodeContainer *)n)->containerInNode != nullptr));
  }


  n->addConnectableNodeListener(this);
  String targetName = (nodeName.isNotEmpty())?nodeName:n->nameParam->stringValue();
  n->nameParam->setValue(getUniqueNameInContainer(targetName));

  addChildControllableContainer(n); //ControllableContainer

  if (!nodeData.isVoid()) n->loadJSONData(nodeData);

  nodeChangeNotifier.addMessage(new NodeChangeMessage(n,true));
  //  nodeContainerListeners.call(&NodeContainerListener::nodeAdded, n);
  return n;
}





bool NodeContainer::removeNode(ConnectableNode * n)
{
  Array<NodeConnection *> relatedConnections = getAllConnectionsForNode(n);

  for (auto &connection : relatedConnections) removeConnection(connection);

  if (n == nullptr){jassertfalse; return false;}
  n->removeConnectableNodeListener(this);
  removeChildControllableContainer(n);

  nodeChangeNotifier.addMessage(new NodeChangeMessage(n,false));
  //  nodeContainerListeners.call(&NodeContainerListener::nodeRemoved, n);
  nodes.removeAllInstancesOf(n);

  n->clear();


  if (NodeContainer * nc = dynamic_cast<NodeContainer*>(n)) nodeContainers.removeFirstMatchingValue(nc);

  n->removeFromAudioGraph();
  //if(NodeManager::getInstanceWithoutCreating() != nullptr)
  //  getAudioGraph()->removeNode(n->audioNode);

  return true;
}

ConnectableNode * NodeContainer::getNodeForName(const String & name)
{
  for (auto &n : nodes)
  {
    if (n->shortName == name) return n;
  }
  return nullptr;
}

void NodeContainer::updateAudioGraph(bool lock) {


  if(!MessageManager::getInstance()->isThisTheMessageThread()){
    if(lock){
      const ScopedLock lk (getAudioGraph()->getCallbackLock());
      getAudioGraph()->suspendProcessing(true);
    }
    else{
      getAudioGraph()->suspendProcessing(true);
    }
    triggerAsyncUpdate();

    return;
  }

  if(lock){
    const ScopedLock lk (getAudioGraph()->getCallbackLock());
    getAudioGraph()->setRateAndBufferSizeDetails(NodeBase::getSampleRate(),NodeBase::getBlockSize());
    getAudioGraph()->prepareToPlay(NodeBase::getSampleRate(),NodeBase::getBlockSize());
    getAudioGraph()->suspendProcessing(false);
  }
  else{
    getAudioGraph()->setRateAndBufferSizeDetails(NodeBase::getSampleRate(),NodeBase::getBlockSize());
    getAudioGraph()->prepareToPlay(NodeBase::getSampleRate(),NodeBase::getBlockSize());
    getAudioGraph()->suspendProcessing(false);
  }

  //  }


}


void NodeContainer::handleAsyncUpdate(){
  if(!isEngineLoadingFile()){
    rebuildTimer.stopTimer();
    updateAudioGraph();

  }
  else{
    rebuildTimer.startTimer(10);


  }
}

int NodeContainer::getNumConnections() {
  return connections.size();
}



var NodeContainer::getJSONData()
{
  var data = ConnectableNode::getJSONData();

  var connectionsData;

  for (auto &c : connections)
  {
    connectionsData.append(c->getJSONData());
  }

//  data.getDynamicObject()->setProperty("nodes", nodesData);
  data.getDynamicObject()->setProperty("connections", connectionsData);

  return data;
}


ParameterContainer *  NodeContainer::addContainerFromVar(const String & /*name*/,const var & data){
//  ConnectableNode * node = addNodeFromJSONData(data);
  NodeType nodeType = NodeFactory::getTypeFromString(data.getProperty("nodeType", var()));
  ConnectableNode * node = NodeFactory::createNode(nodeType);
  if (node->type == NodeType::ContainerInType) containerInNode = (ContainerInNode *)node;
  else if (node->type == NodeType::ContainerOutType) containerOutNode = (ContainerOutNode *)node;
  addNode(node);
  return node;
}

void NodeContainer::loadJSONData(const var & data)
{
  // do we really need that ???
  clear(false);
  NodeBase::loadJSONData(data);


  Array<var> * connectionsData = data.getProperty("connections", var()).getArray();

  if (connectionsData)
  {
    for (var &cData : *connectionsData)
    {

      ConnectableNode * srcNode = (ConnectableNode*)(getNodeForName(cData.getDynamicObject()->getProperty("srcNode").toString())) ;
      ConnectableNode * dstNode = (ConnectableNode*)(getNodeForName(cData.getDynamicObject()->getProperty("dstNode").toString()));

      int cType = cData.getProperty("connectionType", var());

      if (srcNode && dstNode && isPositiveAndBelow(cType, (int)NodeConnection::ConnectionType::UNDEFINED)) {
        NodeConnection * c = addConnection(srcNode, dstNode, NodeConnection::ConnectionType(cType));
        // if c == null connection already exist, should never happen loading JSON but safer to check
        if(c){
          c->loadJSONData(cData);
        }


      }
      else {
        // TODO nicely handle file format errors?

        if(srcNode==nullptr){
          NLOG("loadJSON","!!! no srcnode for shortName : "+cData.getDynamicObject()->getProperty("srcNode").toString());
        }
        if(dstNode==nullptr){
          NLOG("loadJSON","!!! no dstnode for shortName : "+cData.getDynamicObject()->getProperty("dstNode").toString());
        }


#if defined DEBUG
        LOG("!!! Available Nodes in "+ shortName+" : ");
        for (auto &node : nodes)
        {
          LOG("!!! > " + node->getNiceName()+"//"+ node->shortName);
        }
#endif

        jassertfalse;
      }
    }
  }



  removeIllegalConnections();
}


NodeConnection * NodeContainer::getConnectionBetweenNodes(ConnectableNode * sourceNode, ConnectableNode * destNode, NodeConnection::ConnectionType connectionType)
{
  if(sourceNode==nullptr || destNode == nullptr){
    DBG("wrong Connection");
    return nullptr;
  }
  ConnectableNode * tSourceNode =  sourceNode;
  ConnectableNode * tDestNode =  destNode;

  for(auto &c: connections)
  {
    if (c->sourceNode == tSourceNode && c->destNode == tDestNode && c->connectionType == connectionType) return c;
  }

  return nullptr;
}

Array<NodeConnection*> NodeContainer::getAllConnectionsForNode(ConnectableNode * node)
{
  Array<NodeConnection*> result;

  ConnectableNode * tSourceNode = node;
  ConnectableNode * tDestNode = node;

  for (auto &connection : connections)
  {
    if (connection->sourceNode == tSourceNode || connection->destNode == tDestNode)
    {
      result.add(connection);
    }
  }



  return result;
}

NodeConnection * NodeContainer::addConnection(ConnectableNode * sourceNode, ConnectableNode * destNode, NodeConnection::ConnectionType connectionType,NodeConnection::Model * root)
{

  ConnectableNode * tSourceNode = sourceNode;
  ConnectableNode * tDestNode =  destNode;

  if (getConnectionBetweenNodes(tSourceNode, tDestNode, connectionType) != nullptr)
  {
    //connection already exists
    DBG("Connection already exists");
    return nullptr;
  }

  NodeConnection * c = new NodeConnection(tSourceNode, tDestNode, connectionType,root);
  connections.add(c);
  c->addConnectionListener(this);
  //  updateAudioGraph();
  // DBG("Dispatch connection Added from NodeManager");
  nodeChangeNotifier.addMessage(new NodeChangeMessage(c,true));
  //  nodeContainerListeners.call(&NodeContainerListener::connectiosnAdded, c);

  return c;
}


bool NodeContainer::removeConnection(NodeConnection * c)
{
  if (c == nullptr) return false;
  c->removeConnectionListener(this);

  connections.removeObject(c);
  nodeChangeNotifier.addMessage(new NodeChangeMessage(c,false));
  //  nodeContainerListeners.call(&NodeContainerListener::connectionRemoved, c);

  return true;
}



//From NodeBase Listener
void NodeContainer::askForRemoveNode(ConnectableNode * node)
{
  removeNode(node);

}



void NodeContainer::askForRemoveConnection(NodeConnection *connection)
{
  removeConnection(connection);
}

void NodeContainer::RMSChanged(ConnectableNode * node, float _rmsInValue, float _rmsOutValue)
{
  if (node == containerInNode) rmsInValue = _rmsOutValue;
  else if (node == containerOutNode) rmsOutValue = _rmsInValue;

  rmsListeners.call(&ConnectableNode::RMSListener::RMSChanged, this, rmsInValue, rmsOutValue);
}

void NodeContainer::onContainerParameterChanged(Parameter * p)
{

  NodeBase::onContainerParameterChanged(p);

}




void NodeContainer::onContainerParameterChangedAsync(Parameter * p  ,const var & v) {
  NodeBase::onContainerParameterChangedAsync(p ,v);
  if (p == enabledParam)
  {

    triggerAsyncUpdate();
  }

};

void NodeContainer::bypassNode(bool /*bypass*/){}

ConnectableNodeUI * NodeContainer::createUI()
{
  return new NodeContainerUI(this);
}


bool NodeContainer::hasDataInputs()
{
  return containerInNode != nullptr ? containerInNode->hasDataInputs() : false;
}

bool NodeContainer::hasDataOutputs()
{
  return containerOutNode != nullptr ? containerOutNode->hasDataOutputs() : false;
}


void NodeContainer::numChannelsChanged(bool isInput){

  const ScopedLock lk(getCallbackLock());
  removeIllegalConnections();
  getAudioGraph()->setPlayConfigDetails(getTotalNumInputChannels(), getTotalNumOutputChannels(), getSampleRate(), getBlockSize());
  if(isInput && containerInNode)containerInNode->numChannels->setValue(getTotalNumInputChannels());//containerInNode->setPreferedNumAudioOutput(getTotalNumInputChannels());
  else if(!isInput && containerOutNode)containerOutNode->numChannels->setValue(getTotalNumOutputChannels());
}
void NodeContainer::prepareToPlay(double d, int i) {
  NodeBase::prepareToPlay(d, i);


  jassert(getSampleRate());
  jassert(getBlockSize());
  //    numChannelsChanged();
  {
    const ScopedLock lk(innerGraph->getCallbackLock());
    getAudioGraph()->setRateAndBufferSizeDetails(NodeBase::getSampleRate(),NodeBase::getBlockSize());
    getAudioGraph()->prepareToPlay(d, i);
  }




};




void NodeContainer::processBlockInternal(AudioBuffer<float>& buffer , MidiBuffer& midiMessage ) {
#ifdef MULTITHREADED_AUDIO
  if(!nodeManager){
    nodeManager = NodeManager::getInstance();
  }
  if(parentNodeContainer &&(nodeManager->getNumJobs()<4)){
    const ScopedLock lk(innerGraph->getCallbackLock());
    graphJob->setBuffersToReferTo(buffer, midiMessage);
    nodeManager->addJob(graphJob,false);
    while(nodeManager->contains(graphJob)){}
  }
  else{
    const ScopedLock lk(innerGraph->getCallbackLock());
    getAudioGraph()->processBlock(buffer,midiMessage);
  }
#else
  const ScopedLock lk(innerGraph->getCallbackLock());
  getAudioGraph()->processBlock(buffer,midiMessage);
#endif
};
void NodeContainer::processBlockBypassed(AudioBuffer<float>& /*buffer*/, MidiBuffer& /*midiMessages*/){
  //    getAudioGraph()->processBlockBypassed(buffer,midiMessages);
  
}



void NodeContainer::removeIllegalConnections() {
  getAudioGraph()->removeIllegalConnections();
  for(auto & c:nodeContainers){
    c->removeIllegalConnections();
  }
  
}
