/*
 ==============================================================================

 NodeContainer.cpp
 Created: 18 May 2016 7:53:56pm
 Author:  bkupe

 ==============================================================================
 */

#include "NodeContainer.h"
#include "NodeManager.h"
#include "NodeConnection.h"
#include "NodeContainerUI.h"


#include "DebugHelpers.h"



AudioDeviceManager& getAudioDeviceManager();
bool isEngineLoadingFile();

NodeContainer::NodeContainer(const String &name) :
ConnectableNode("Container",NodeType::ContainerType,true),
containerInNode(nullptr),
containerOutNode(nullptr)
{
  saveAndLoadRecursiveData = false;
  
}


NodeContainer::~NodeContainer()
{
  //connections.clear();
  clear(false);
}

AudioProcessorGraph * NodeContainer::getAudioGraph(){
  if(NodeManager * nm = NodeManager::getInstanceWithoutCreating())
  return nm->mainAudioGraph;

  return nullptr;
}
void NodeContainer::clear(bool recreateContainerNodes)
{

  while (connections.size() > 0)
  {
    connections[0]->remove();
  }

  while (nodes.size() > 0)
  {
    nodes[0]->remove();

  }


  containerInNode = nullptr;
  containerOutNode = nullptr;


  if (recreateContainerNodes && parentNodeContainer != nullptr)
  {
    containerInNode = (ContainerInNode *)addNode(new ContainerInNode());
    containerOutNode = (ContainerOutNode *)addNode(new ContainerOutNode());

    containerInNode->xPosition->setValue(150);
    containerInNode->yPosition->setValue(100);
    containerOutNode->xPosition->setValue(450);
    containerOutNode->yPosition->setValue(100);

    //maybe keep it ?
    addConnection(containerInNode, containerOutNode, NodeConnection::ConnectionType::AUDIO);
  }

  while (proxyParams.size() > 0)
  {
    removeParamProxy(proxyParams[0]);
  }
  if(!recreateContainerNodes)ConnectableNode::clear();


}


ConnectableNode * NodeContainer::addNode(NodeType nodeType, const String &nodeName)
{
  ConnectableNode * n = NodeFactory::createNode(nodeType);
  return addNode(n,nodeName);
}

ConnectableNode * NodeContainer::addNode(ConnectableNode * n, const String &nodeName)
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

  nodeContainerListeners.call(&NodeContainerListener::nodeAdded, n);
  return n;
}



bool NodeContainer::removeNode(ConnectableNode * n)
{
  Array<NodeConnection *> relatedConnections = getAllConnectionsForNode(n);

  for (auto &connection : relatedConnections) removeConnection(connection);

  if (n == nullptr){jassertfalse; return false;}
  n->removeConnectableNodeListener(this);
  removeChildControllableContainer(n);

  nodeContainerListeners.call(&NodeContainerListener::nodeRemoved, n);
  nodes.removeAllInstancesOf(n);

  n->clear();
  n->removeFromAudioGraph();

  if (NodeContainer * nc = dynamic_cast<NodeContainer*>(n)) nodeContainers.removeFirstMatchingValue(nc);

  //if(NodeManager::getInstanceWithoutCreating() != nullptr)
  getAudioGraph()->removeNode(n->audioNode);

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

const ScopedLock lkp (getAudioGraph()->getCallbackLock());
  AudioIODevice *ad= getAudioDeviceManager().getCurrentAudioDevice();
   getAudioGraph()->prepareToPlay(ad->getCurrentSampleRate(),ad->getCurrentBufferSizeSamples());
//
//  if(parentNodeContainer){
//    const ScopedLock lkp (getAudioGraph()->getCallbackLock());
//    //    const ScopedLock lk( getAudioGraph()->getCallbackLock());
////    getAudioGraph()->setRateAndBufferSizeDetails(NodeBase::getSampleRate(),NodeBase::getBlockSize());
//
//  }
//  else {
//    // if no parent we are an audiograph inside gobal graphplayer
//    const ScopedLock lk( getAudioGraph()->getCallbackLock());
//    getAudioGraph()->prepareToPlay(NodeBase::getSampleRate(),NodeBase::getBlockSize());
//
//  }


}


int NodeContainer::getNumConnections() {
  return connections.size();
}

ParameterProxy * NodeContainer::addParamProxy()
{
  ParameterProxy * p = new ParameterProxy();
  ControllableContainer::addParameter(p);
  proxyParams.add(p);
  p->addParameterProxyListener(this);
  nodeContainerListeners.call(&NodeContainerListener::paramProxyAdded, p);

  return p;
}

void NodeContainer::removeParamProxy(ParameterProxy * pp)
{
  pp->removeParameterProxyListener(this);

  nodeContainerListeners.call(&NodeContainerListener::paramProxyRemoved, pp);
  proxyParams.removeAllInstancesOf(pp);
  removeControllable(pp);
}


bool NodeContainer::loadPreset(PresetManager::Preset * preset)
{
  if(!ControllableContainer::loadPreset(preset)) return false;

  //    for (auto &n : nodes) n->loadPresetWithName(preset->name);

  return true;
}

PresetManager::Preset* NodeContainer::saveNewPreset(const String & name)
{
  return ControllableContainer::saveNewPreset(name);
  //    for (auto &n : nodes) n->saveNewPreset(name);
}

bool NodeContainer::saveCurrentPreset()
{
  if (!ControllableContainer::saveCurrentPreset()) return false;
  //@ben remove recursive for now (not useful and duplicating values...)
  //
  //    for (auto &n : nodes) n->saveCurrentPreset();
  return true;
}

bool NodeContainer::resetFromPreset()
{
  if (!ControllableContainer::resetFromPreset()) return false;

  //    for (auto &n : nodes) n->resetFromPreset();

  return true;
}



var NodeContainer::getJSONData()
{

  var data = ConnectableNode::getJSONData();
  var nodesData;

  for (auto &n : nodes)
  {
    nodesData.append(n->getJSONData());
  }

  var connectionsData;

  for (auto &c : connections)
  {
    connectionsData.append(c->getJSONData());
  }

  data.getDynamicObject()->setProperty("nodes", nodesData);
  data.getDynamicObject()->setProperty("connections", connectionsData);



  var proxiesData;
  for (auto &pp : proxyParams)
  {
    proxiesData.append(pp->getJSONData());
  }

  data.getDynamicObject()->setProperty("proxies", proxiesData);

  return data;
}


void NodeContainer::loadJSONDataInternal(var data)
{
  clear(false);

  Array<var> * nodesData = data.getProperty("nodes", var()).getArray();
  if(nodesData!=nullptr){
    for (var &nData : *nodesData)
    {
      addNodeFromJSON(nData);
    }
  }

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
          NLOG("loadJSON","no srcnode for shortName : "+cData.getDynamicObject()->getProperty("srcNode").toString());
        }
        if(dstNode==nullptr){
          NLOG("loadJSON","no dstnode for shortName : "+cData.getDynamicObject()->getProperty("dstNode").toString());
        }
        LOG("Available Nodes in "+ shortName+" : ");

#if defined DEBUG
        for (auto &node : nodes)
        {
          DBG(" > " + node->niceName+"//"+ node->shortName);
        }
#endif

        jassertfalse;
      }
    }
  }





  Array<var> * proxiesData = data.getProperty("proxies", var()).getArray();

  if (proxiesData)
  {
    for (var &pData : *proxiesData)
    {
      ParameterProxy * p = addParamProxy();
      p->loadJSONData(pData);
    }
  }


  removeIllegalConnections();
}

ConnectableNode * NodeContainer::addNodeFromJSON(var nodeData)
{
  Array<var> * params = nodeData.getDynamicObject()->getProperty("parameters").getArray();
  String sourceName = "";
  for (auto &v : *params)
  {

    if (v.getDynamicObject()->getProperty(controlAddressIdentifier) == "/name")
    {
      sourceName = v.getDynamicObject()->getProperty("value").toString();
      break;
    }
  }


  NodeType nodeType = NodeFactory::getTypeFromString(nodeData.getProperty("nodeType", var()));
  ConnectableNode * node = addNode(nodeType, sourceName);
  String safeNodeName = node->niceName;

  if (node->type == NodeType::ContainerInType)
  {
    containerInNode = (ContainerInNode *)node;

  } else if (node->type == NodeType::ContainerOutType)
  {
    containerOutNode = (ContainerOutNode *)node;
  }


  node->loadJSONData(nodeData);
  node->nameParam->setValue(safeNodeName); //@martin new naming now takes into account the original node name

  nodeContainerListeners.call(&NodeContainerListener::nodeAdded, node);

  return node;

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

NodeConnection * NodeContainer::addConnection(ConnectableNode * sourceNode, ConnectableNode * destNode, NodeConnection::ConnectionType connectionType)
{

  ConnectableNode * tSourceNode = sourceNode;
  ConnectableNode * tDestNode =  destNode;

  if (getConnectionBetweenNodes(tSourceNode, tDestNode, connectionType) != nullptr)
  {
    //connection already exists
    DBG("Connection already exists");
    return nullptr;
  }

  NodeConnection * c = new NodeConnection(tSourceNode, tDestNode, connectionType);
  connections.add(c);
  c->addConnectionListener(this);
//  updateAudioGraph();
  // DBG("Dispatch connection Added from NodeManager");
  nodeContainerListeners.call(&NodeContainerListener::connectionAdded, c);

  return c;
}


bool NodeContainer::removeConnection(NodeConnection * c)
{
  if (c == nullptr) return false;
  c->removeConnectionListener(this);

  connections.removeObject(c);

  nodeContainerListeners.call(&NodeContainerListener::connectionRemoved, c);

  return true;
}



//From NodeBase Listener
void NodeContainer::askForRemoveNode(ConnectableNode * node)
{
  if(    removeNode(node)){
    // TODO we should give a pre_delete() function to nodes for unregistering all listener and stuffs,
    // for now re opening a complex session gives crash on the second open as they are not being deleted by us but the AudioGraph
    //        if(NodeBase *nb=dynamic_cast<NodeBase *>(node)){
    //         nb->stopTimer();
    //        }
  }
}

void NodeContainer::askForRemoveProxy(ParameterProxy * p)
{
  removeParamProxy(p);
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
  ConnectableNode::onContainerParameterChanged(p);

}

void NodeContainer::onContainerParameterChangedAsync(Parameter * p  ,const var & v) {
  ConnectableNode::onContainerParameterChangedAsync(p ,v);

};

void NodeContainer::bypassNode(bool bypass){}

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








void NodeContainer::removeIllegalConnections() {
  getAudioGraph()->removeIllegalConnections();
//  for(auto & c:nodeContainers){
//    c->removeIllegalConnections();
//  }

}
