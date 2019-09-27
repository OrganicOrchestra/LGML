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

#if JUCE_DEBUG && 1
#define DBGGRAPH(x) {int d = 0; auto * cpa = parentContainer;while(cpa){d++;cpa=cpa->parentContainer;};DBG(String::repeatedString(">",d+1)+" "+x);}
#else
#define DBGGRAPH(x)
#endif

#define REBUILD_WAIT 10 // the time  in ms to wait before updating the graph if not ready


extern AudioDeviceManager& getAudioDeviceManager();
extern bool isEngineLoadingFile();

class GraphBuildWatcher : public AsyncUpdater{
    public :
    GraphBuildWatcher(NodeContainer* _nc):nc(_nc){
        startBuild();
    }
    void startBuild(){
        nc->getAudioGraph()->suspendProcessing(true);
        nc->getAudioGraph()->prepareToPlay(nc->NodeBase::getSampleRate(), nc->NodeBase::getBlockSize());
        triggerAsyncUpdate(); // this update should occure just after the one triggered by reparetoplay
    }
    void handleAsyncUpdate() override{
        nc->getAudioGraph()->suspendProcessing(false);
        nc->gWatcher=nullptr;
    };
    NodeContainer* nc;
};


NodeContainer::NodeContainer (StringRef name):NodeContainer(name,false){
    canHaveUserDefinedContainers = true;

}
NodeContainer::NodeContainer (StringRef name,bool _isRoot) :
containerInNode (nullptr)
,containerOutNode (nullptr)
,NodeBase (name, false)
,nodeChangeNotifier (10000)
,rebuildTimer (this)
,isRoot(_isRoot)
,innerGraph( new AudioProcessorGraph())
,userContainer ("vars")
{
    canHaveUserDefinedContainers = true;
    userContainer.setUserDefined (true);
    userContainer.addControllableContainerListener (this);
    addChildControllableContainer (&userContainer);
    userContainer.nameParam->setInternalOnlyFlags(true,true);

    innerGraph->releaseResources();
    setBuildSessionGraph(false);
    setPreferedNumAudioOutput (2);
    setPreferedNumAudioInput (2);
#ifdef MULTITHREADED_AUDIO
    graphJob = new GraphJob (innerGraph, name);
    nodeManager = nullptr;
#endif

    //Force non recursive saving of preset as container has only is level to take care, nested containers are other nodes
    _presetSavingIsRecursive = false;


    //maybe keep it ?
    //    addConnection (containerInNode, containerOutNode, NodeConnection::ConnectionType::AUDIO);
}


NodeContainer::~NodeContainer()
{
    masterReference.clear();
    //connections.clear();
    rebuildTimer.stopTimer();
    clear ();

    innerGraph->releaseResources();

}



void NodeContainer::clear ()
{
    DBGGRAPH(getNiceName()+" clear Container");

    if(connections.size() || nodeContainers.size() || nodes.size()){
        if(auto * g = getAudioGraph())g->suspendProcessing(true);
        setBuildSessionGraph(true);

        while (connections.size() > 0)
        {
            connections[0]->remove();
        }

        while(nodeContainers.size()){
            auto * n = nodeContainers[0];
            removeNode(n);
        }



        nodes.clear();
        setBuildSessionGraph(false);
        //        updateAudioGraph(true);


        if(auto * g = getAudioGraph())g->clear();
    }








    //    setPreferedNumAudioOutput (2);
    //    setPreferedNumAudioInput (2);


    ConnectableNode::clear();



}




ConnectableNode* NodeContainer::addNodeFromJSONData (DynamicObject* data)
{
    ConnectableNode* n = NodeFactory::createBaseFromObject ("", data,false);
    return addNode (n, n->getNiceName(), data);
}

ConnectableNode* NodeContainer::addNode (ConnectableNode* n, const String& nodeName, DynamicObject* nodeData)
{
    DBGGRAPH(getNiceName()+" : adding Node "+n->getNiceName() + ": " + nodeName);
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


void NodeContainer::addToAudioGraph (NodeBase* n)
{
    DBGGRAPH(getNiceName()+" : adding "+(dynamic_cast<NodeContainer*>(n)?"Container":"Node")+" to graph "+n->getNiceName()+(isBuildingSession.get()?"(building)":""));
    if(auto * g = getAudioGraph()){
        n->audioNode = g->addNode (std::unique_ptr<AudioProcessor>(n->getAudioProcessor()));
        updateAudioGraph();
    }
    //    jassert(g->getSampleRate()!=0 && g->getBlockSize()!=0);
    //    getAudioProcessor()->setRateAndBufferSizeDetails (g->getSampleRate(), g->getBlockSize());

}

bool NodeContainer::isParentBuildingSession(){
    if(parentNodeContainer && !parentNodeContainer->isBuildingSession.get()){
        return parentNodeContainer->isParentBuildingSession();
    }
    return true;

}
void NodeContainer::removeFromAudioGraph(NodeBase * n)
{
    jassert(getAudioGraph());
    DBGGRAPH(getNiceName()+" : removing Node from graph "+n->getNiceName());
    if (auto pG = getAudioGraph())
    {
        const ScopedLock lk (pG->getCallbackLock());
        pG->removeNode(n->audioNode);
    }

    updateAudioGraph (true); // we need to force rebuild on deletion to avoid memory leaks


}


void NodeContainer::setBuildSessionGraph(bool state){
    jassert(!state || (state!=(bool)(isBuildingSession.get()))); 
    DBGGRAPH(String(":::::::::::") +getNiceName()+" : "+(state?"start":"stop")+" building graph");
    isBuildingSession = state?1:0;

}
bool NodeContainer::removeNode (ConnectableNode* n,bool doDelete)
{
    DBGGRAPH(getNiceName()+" : removing Node"+n->getNiceName() + (doDelete?" deleting":""));
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

void NodeContainer::updateAudioGraph (bool force)
{

    DBGGRAPH(getNiceName()+(isBuildingSession.get()?"(in session)":"")+" : updating Graph "+(force?"forced":""));

    if(!force && isBuildingSession.get()){
        rebuildTimer.startTimer (REBUILD_WAIT);
        return;
    }

    if(NodeBase::getBlockSize()==0 || NodeBase::getSampleRate()==0){
        //            jassertfalse;
        // node is not ready , postponing setup
        if( !isEngineLoadingFile()) {
            LOGW(juce::translate("node 123 is not ready , postponing setup").replace("123",getNiceName()));
            //                jassertfalse;
        }
        else{
            rebuildTimer.startTimer (REBUILD_WAIT);
        }


    }
    else{
        {
//            MessageManagerLock mm;
            if(gWatcher){
                gWatcher->cancelPendingUpdate();
                gWatcher->startBuild();
            }
            else{
                gWatcher = std::make_unique< GraphBuildWatcher>(this);
            }
        }
        cancelPendingUpdate();
        setBuildSessionGraph(false);

    }

}


void NodeContainer::handleAsyncUpdate()
{

    if ( isBuildingSession.get()){
        DBGGRAPH("postponing :" + getNiceName());
        rebuildTimer.startTimer (REBUILD_WAIT);
    }
    else
    {
        rebuildTimer.stopTimer();
        updateAudioGraph(true);
    }

}

int NodeContainer::getNumConnections()
{
    return connections.size();
}



DynamicObject* NodeContainer::createObject()
{
    auto data = ConnectableNode::createObject();

    var connectionsData;

    for (auto& c : connections)
    {
        connectionsData.append (c->createObject());
    }


    data->setProperty ("connections", connectionsData);

    return data;
}


ParameterContainer*   NodeContainer::addContainerFromObject (const String& name, DynamicObject*   data)
{
    //  ConnectableNode * node = addNodeFromJSONData(data);

    ConnectableNode* node = NodeFactory::createBaseFromObject ( "", data,false); // avoid to configure it before added
    if(!node){jassertfalse; return nullptr;}
    if (auto n = dynamic_cast<ContainerInNode*> (node)) containerInNode = n;
    else if (auto n = dynamic_cast<ContainerOutNode*> (node)) containerOutNode = n;
    if(data && data->hasProperty(uidIdentifier)){
        node->uid = data->getProperty(uidIdentifier).toString();
    }
    addNode (node,name,data);
    return node;
}

void NodeContainer::configureFromObject (DynamicObject* data)
{

    setBuildSessionGraph(true);



    NodeBase::configureFromObject (data);

    setBuildSessionGraph(false);
    //    updateAudioGraph(true);
    if( auto *  connectionsData = data->getProperty ("connections").getArray()){
        setConnectionFromObject(*connectionsData);
    }



}

void NodeContainer::setConnectionFromObject(const Array<var> & connectionsData){
    // save connection and remove them from object to pass valid object to NodeBaseParsing

        for (const var& cData : connectionsData)
        {

            ConnectableNode* srcNode = (ConnectableNode*) (getControllableContainerByShortName(cData.getDynamicObject()->getProperty ("srcNode").toString())) ;
            ConnectableNode* dstNode = (ConnectableNode*) (getControllableContainerByShortName (cData.getDynamicObject()->getProperty ("dstNode").toString()));

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




void NodeContainer::numChannelsChanged (bool isInput)
{
    DBGGRAPH(getNiceName()+" : numChannels Changed :"+(isInput?"i":"o"));
    const ScopedLock lk (getCallbackLock());
    removeIllegalConnections();
    getAudioGraph()->setPlayConfigDetails (getTotalNumInputChannels(), getTotalNumOutputChannels(), getSampleRate(), getBlockSize());

    if (isInput && containerInNode)containerInNode->numChannels->setValue (getTotalNumInputChannels()); //containerInNode->setPreferedNumAudioOutput(getTotalNumInputChannels());
    else if (!isInput && containerOutNode)containerOutNode->numChannels->setValue (getTotalNumOutputChannels());
}


void NodeContainer::prepareToPlay (double d, int i)
{
    DBGGRAPH(getNiceName()+" : prepare to play");
    NodeBase::prepareToPlay (d, i);
    updateAudioGraph();
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
    if( auto * g = getAudioGraph())g->removeIllegalConnections();
    
    for (auto& c : nodeContainers)
    {
        c->removeIllegalConnections();
    }
    
}
