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

#include "../../Utils/DebugHelpers.h"

REGISTER_NODE_TYPE (NodeContainer)

extern AudioDeviceManager& getAudioDeviceManager();
extern bool isEngineLoadingFile();


NodeContainer::NodeContainer (StringRef name):NodeContainer(name,false){

}
NodeContainer::NodeContainer (StringRef name,bool _isRoot) :
containerInNode (nullptr),
containerOutNode (nullptr),
NodeBase (name, false),
nodeChangeNotifier (10000),
rebuildTimer (this),
isRoot(_isRoot)
{

    innerGraph = new AudioProcessorGraph();
    innerGraph->releaseResources();
    setPreferedNumAudioOutput (2);
    setPreferedNumAudioInput (2);
#ifdef MULTITHREADED_AUDIO
    graphJob = new GraphJob (innerGraph, name);
    nodeManager = nullptr;
#endif

    //Force non recursive saving of preset as container has only is level to take care, nested containers are other nodes
    presetSavingIsRecursive = false;


    //maybe keep it ?
    //    addConnection (containerInNode, containerOutNode, NodeConnection::ConnectionType::AUDIO);
}


NodeContainer::~NodeContainer()
{
    //connections.clear();
    rebuildTimer.stopTimer();
    clear ();
    
    innerGraph->releaseResources();
    
}



void NodeContainer::clear ()
{

    while (connections.size() > 0)
    {
        connections[0]->remove();
    }



    nodes.clear();



    //    setPreferedNumAudioOutput (2);
    //    setPreferedNumAudioInput (2);


    ConnectableNode::clear();



}




ConnectableNode* NodeContainer::addNodeFromJSONData (DynamicObject* data)
{
    ConnectableNode* n = NodeFactory::createBaseFromObject (String::empty, data);
    return addNode (n, n->getNiceName(), data);
}

ConnectableNode* NodeContainer::addNode (ConnectableNode* n, const String& nodeName, DynamicObject* nodeData)
{
    nodes.add ((NodeBase*)n);



    if (NodeContainer* nc = dynamic_cast<NodeContainer*> (n))
    {
        nodeContainers.add (nc);
        //        nc->NodeContainer::clear (!isEngineLoadingFile());
        //DBG("Check containerIn Node : " << String(((NodeContainer *)n)->containerInNode != nullptr));
    }

    String targetName = getUniqueNameInContainer (nodeName.isNotEmpty() ? nodeName : n->nameParam->stringValue());
    n->nameParam->setValue (targetName);

    addChildControllableContainer (n); //ControllableContainer


    n->setParentNodeContainer (this);

    if (nodeData) n->configureFromObject (nodeData);

    
    nodeChangeNotifier.addMessage (new NodeChangeMessage (n, true));
    //  nodeContainerListeners.call(&NodeContainerListener::nodeAdded, n);
    return n;
}





bool NodeContainer::removeNode (ConnectableNode* n,bool doDelete)
{
    Array<NodeConnection*> relatedConnections = getAllConnectionsForNode (n);

    for (auto& connection : relatedConnections) removeConnection (connection);

    if (n == nullptr) {jassertfalse; return false;}

    nodeChangeNotifier.addMessage (new NodeChangeMessage (n, false));
    nodeContainerListeners.call(&NodeContainerListener::nodeRemoved, n);
    
    if(!doDelete)removeChildControllableContainer (n);



    if(nodes.contains((NodeBase*)n)){
        n->clear();
    }
    else{
        // node has already been deleted
        //jassertfalse;
        return false;
    }



    if (NodeContainer* nc = dynamic_cast<NodeContainer*> (n)) nodeContainers.removeFirstMatchingValue (nc);

    if(doDelete)
        nodes.removeObject ((NodeBase*)n);
    //if(NodeManager::getInstanceWithoutCreating() != nullptr)
    //  getAudioGraph()->removeNode(n->audioNode);

    return true;
}

ConnectableNode* NodeContainer::getNodeForName (const String& _name)
{
    const String name = _name.toLowerCase();
    for (auto& n : nodes)
    {
        if (n->shortName == name) return n;
    }

    return nullptr;
}

void NodeContainer::updateAudioGraph (bool lock)
{


    if (!MessageManager::getInstance()->isThisTheMessageThread())
    {

        {
            ScopedPointer<ScopedLock> lk;
            if(lock){lk = new ScopedLock(getAudioGraph()->getCallbackLock());}

            getAudioGraph()->suspendProcessing (true);
        }


        triggerAsyncUpdate();

        return;
    }

    {
        ScopedPointer<ScopedLock> lk;
        if(lock){lk = new ScopedLock(getAudioGraph()->getCallbackLock());}


        if(NodeBase::getBlockSize()==0 || NodeBase::getSampleRate()==0){
            //            jassertfalse;
            // node is not ready , postponing setup
            if( !isEngineLoadingFile()) {
                LOGW(juce::translate("node 123 is not ready , postponing setup").replace("123",getNiceName()));
//                jassertfalse;
            }
            else{
                rebuildTimer.startTimer (100);
            }
            

        }
        else{
//            LOG("! node 123 is ready"+getNiceName()+" is ready");
            getAudioGraph()->setRateAndBufferSizeDetails (NodeBase::getSampleRate(), NodeBase::getBlockSize());
            getAudioGraph()->prepareToPlay (NodeBase::getSampleRate(), NodeBase::getBlockSize());
            getAudioGraph()->suspendProcessing (false);
        }
    }




}


void NodeContainer::handleAsyncUpdate()
{
    if (!isEngineLoadingFile())
    {
        rebuildTimer.stopTimer();
        updateAudioGraph();

    }
    else
    {
        rebuildTimer.startTimer (100);


    }
}

int NodeContainer::getNumConnections()
{
    return connections.size();
}



DynamicObject* NodeContainer::getObject()
{
    auto data = ConnectableNode::getObject();

    var connectionsData;

    for (auto& c : connections)
    {
        connectionsData.append (c->getObject());
    }

    //  data.getDynamicObject()->setProperty("nodes", nodesData);
    data->setProperty ("connections", connectionsData);

    return data;
}


ParameterContainer*   NodeContainer::addContainerFromObject (const String& /*name*/, DynamicObject*   data)
{
    //  ConnectableNode * node = addNodeFromJSONData(data);

    ConnectableNode* node = NodeFactory::createBaseFromObject ( String::empty, data);

    if (auto n = dynamic_cast<ContainerInNode*> (node)) containerInNode = n;
    else if (auto n = dynamic_cast<ContainerOutNode*> (node)) containerOutNode = n;

    addNode (node);
    return node;
}

void NodeContainer::configureFromObject (DynamicObject* data)
{
    // do we really need that ???
    //    clear (false);





    NodeBase::configureFromObject (data);



    // save connection and remove them from object to pass valid object to NodeBaseParsing
    Array<var>* _connectionsData = data->getProperty ("connections").getArray();
    Array<var> connectionsData;
    if(_connectionsData){
        for (auto &v:*_connectionsData){
            connectionsData.add(v);
        }
    }

    for (var& cData : connectionsData)
    {

        ConnectableNode* srcNode = (ConnectableNode*) (getNodeForName (cData.getDynamicObject()->getProperty ("srcNode").toString())) ;
        ConnectableNode* dstNode = (ConnectableNode*) (getNodeForName (cData.getDynamicObject()->getProperty ("dstNode").toString()));

        int cType = cData.getProperty ("connectionType", var());

        if (srcNode && dstNode && isPositiveAndBelow (cType, (int)NodeConnection::ConnectionType::UNDEFINED))
        {
            NodeConnection* c = addConnection (srcNode, dstNode, NodeConnection::ConnectionType (cType));

            // if c == null connection already exist, should never happen loading JSON but safer to check
            if (c)
            {
                c->configureFromObject (cData.getDynamicObject());
            }


        }
        else
        {
            // TODO nicely handle file format errors?

            if (srcNode == nullptr)
            {
                NLOGE ("loadJSON", juce::translate("no srcnode for shortName : ") + cData.getDynamicObject()->getProperty ("srcNode").toString());
            }

            if (dstNode == nullptr)
            {
                NLOGE ("loadJSON", juce::translate("no dstnode for shortName : ") + cData.getDynamicObject()->getProperty ("dstNode").toString());
            }


#if defined DEBUG
            LOGE(juce::translate("Available Nodes in " )+ shortName + " : ");

            for (auto& node : nodes)
            {
                LOGE("> " + node->getNiceName() + "//" + node->shortName);
            }

#endif

            jassertfalse;
        }
    }




    removeIllegalConnections();
}


NodeConnection* NodeContainer::getConnectionBetweenNodes (ConnectableNode* sourceNode, ConnectableNode* destNode, NodeConnection::ConnectionType connectionType)
{
    if (sourceNode == nullptr || destNode == nullptr)
    {
        DBG ("wrong Connection");
        return nullptr;
    }

    ConnectableNode* tSourceNode =  sourceNode;
    ConnectableNode* tDestNode =  destNode;

    for (auto& c : connections)
    {
        if (c->sourceNode == tSourceNode && c->destNode == tDestNode && c->connectionType == connectionType) return c;
    }

    return nullptr;
}

Array<NodeConnection*> NodeContainer::getAllConnectionsForNode (ConnectableNode* node)
{
    Array<NodeConnection*> result;

    ConnectableNode* tSourceNode = node;
    ConnectableNode* tDestNode = node;

    for (auto& connection : connections)
    {
        if (connection->sourceNode == tSourceNode || connection->destNode == tDestNode)
        {
            result.add (connection);
        }
    }



    return result;
}

NodeConnection* NodeContainer::addConnection (ConnectableNode* sourceNode, ConnectableNode* destNode, NodeConnection::ConnectionType connectionType, NodeConnection::Model* root)
{

    ConnectableNode* tSourceNode = sourceNode;
    ConnectableNode* tDestNode =  destNode;

    if (getConnectionBetweenNodes (tSourceNode, tDestNode, connectionType) != nullptr)
    {
        //connection already exists
        DBG ("Connection already exists");
        return nullptr;
    }

    NodeConnection* c = new NodeConnection (tSourceNode, tDestNode, connectionType, root);
    connections.add (c);
    c->addConnectionListener (this);
    //  updateAudioGraph();
    // DBG("Dispatch connection Added from NodeManager");
    nodeChangeNotifier.addMessage (new NodeChangeMessage (c, true));
    //  nodeContainerListeners.call(&NodeContainerListener::connectiosnAdded, c);

    return c;
}


bool NodeContainer::removeConnection (NodeConnection* c)
{
    if (c == nullptr) return false;

    c->removeConnectionListener (this);

    connections.removeObject (c);
    nodeChangeNotifier.addMessage (new NodeChangeMessage (c, false));
    //  nodeContainerListeners.call(&NodeContainerListener::connectionRemoved, c);

    return true;
}



//From NodeBase Listener



void NodeContainer::onContainerParameterChanged ( ParameterBase* p)
{

    NodeBase::onContainerParameterChanged (p);

}




void NodeContainer::onContainerParameterChangedAsync ( ParameterBase* p, const var& v)
{
    NodeBase::onContainerParameterChangedAsync (p, v);

    if (p == enabledParam)
    {

        triggerAsyncUpdate();
    }

};

void NodeContainer::bypassNode (bool /*bypass*/) {}



bool NodeContainer::hasDataInputs()
{
    return containerInNode != nullptr ? containerInNode->hasDataInputs() : false;
}

bool NodeContainer::hasDataOutputs()
{
    return containerOutNode != nullptr ? containerOutNode->hasDataOutputs() : false;
}


void NodeContainer::numChannelsChanged (bool isInput)
{

    const ScopedLock lk (getCallbackLock());
    removeIllegalConnections();
    getAudioGraph()->setPlayConfigDetails (getTotalNumInputChannels(), getTotalNumOutputChannels(), getSampleRate(), getBlockSize());

    if (isInput && containerInNode)containerInNode->numChannels->setValue (getTotalNumInputChannels()); //containerInNode->setPreferedNumAudioOutput(getTotalNumInputChannels());
    else if (!isInput && containerOutNode)containerOutNode->numChannels->setValue (getTotalNumOutputChannels());
}
void NodeContainer::prepareToPlay (double d, int i)
{
    NodeBase::prepareToPlay (d, i);
    updateAudioGraph(true);
    if(!isRoot ){

        if(!getContainersOfType<ContainerInNode>(false).size()){

            containerInNode = (ContainerInNode*)addNode (new ContainerInNode());
        }

        if( !getContainersOfType<ContainerOutNode>(false).size()){
            containerOutNode = (ContainerOutNode*)addNode (new ContainerOutNode());
        }



    }





};




void NodeContainer::processBlockInternal (AudioBuffer<float>& buffer, MidiBuffer& midiMessage )
{
#ifdef MULTITHREADED_AUDIO

    if (!nodeManager)
    {
        nodeManager = NodeManager::getInstance();
    }

    if (parentNodeContainer && (nodeManager->getNumJobs() < 4))
    {
        const ScopedLock lk (innerGraph->getCallbackLock());
        graphJob->setBuffersToReferTo (buffer, midiMessage);
        nodeManager->addJob (graphJob, false);

        while (nodeManager->contains (graphJob)) {}
    }
    else
    {
        const ScopedLock lk (innerGraph->getCallbackLock());
        getAudioGraph()->processBlock (buffer, midiMessage);
    }
    
#else
    const ScopedLock lk (innerGraph->getCallbackLock());
    getAudioGraph()->processBlock (buffer, midiMessage);
#endif
};
void NodeContainer::processBlockBypassed (AudioBuffer<float>& /*buffer*/, MidiBuffer& /*midiMessages*/)
{
    //    getAudioGraph()->processBlockBypassed(buffer,midiMessages);
    
}



void NodeContainer::removeIllegalConnections()
{
    getAudioGraph()->removeIllegalConnections();
    
    for (auto& c : nodeContainers)
    {
        c->removeIllegalConnections();
    }
    
}
