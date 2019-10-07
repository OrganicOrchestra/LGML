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


#include "NodeConnection.h"
#include "../Manager/NodeManager.h"
#include "../NodeContainer/NodeContainer.h"
#include "../ConnectableNode.h"
IMPL_OBJ_TYPE (NodeConnection)

NodeBase* getAsNodeBase (WeakReference<ConnectableNode>& n)
{
    return (NodeBase*) n.get();
}
NodeConnection::NodeConnection (ConnectableNode* _sourceNode, ConnectableNode* _destNode, NodeConnection::ConnectionType _connectionType, NodeConnection::Model* root) :
    sourceNode (_sourceNode),
    destNode (_destNode),
    connectionType (_connectionType)
{


    if (connectionType == AUDIO)
    {
        // init with all possible Audio connections
        if (!root)
        {
            int maxCommonAudioConnections = jmin (getAsNodeBase (sourceNode)->getTotalNumOutputChannels(),
                                                  getAsNodeBase (destNode)->getTotalNumInputChannels());

            for (int i = 0; i < maxCommonAudioConnections; i++)
            {
                addAudioGraphConnection (i, i);
            }
        }
        else
        {
            for (auto& c : root->audioConnections)
            {
                addAudioGraphConnection (c.first, c.second);
            }
        }
    }




    (sourceNode)->addConnectableNodeListener (this);
    (destNode)->addConnectableNodeListener (this);




}

NodeConnection::~NodeConnection()
{
    if (connectionType == AUDIO)
    {
        removeAllAudioGraphConnections();
    }



    if (sourceNode.get())
    {

        (sourceNode.get())->removeConnectableNodeListener (this);

    }

    if (destNode.get())
    {

        (destNode.get())->removeConnectableNodeListener (this);

    }

    NodeConnection::masterReference.clear();
}
AudioProcessorGraph* NodeConnection::getParentGraph()
{
    jassert (sourceNode->getParentNodeContainer() == destNode->getParentNodeContainer() );
    return sourceNode->getParentNodeContainer()->getAudioGraph();

}

bool NodeConnection::addAudioGraphConnection (uint32 sourceChannel, uint32 destChannel)
{

    bool result = true;

    if (AudioProcessorGraph* g = getParentGraph())
    {
        result = g->addConnection (AudioProcessorGraph::Connection(
                                                                   {getAsNodeBase (sourceNode)->getAudioNode()->nodeID,
                                                                    (int)sourceChannel},
                                                                   {getAsNodeBase (destNode)->getAudioNode()->nodeID,
                                                                    (int)destChannel}
                                                                   )
                                   );
    }
    else
    {
        jassertfalse;
        result = false;
    }


    if (result)
    {
        AudioConnection ac = AudioConnection (sourceChannel, destChannel);
        model.audioConnections.add (ac);
        listeners.call (&Listener::connectionAudioLinkAdded, ac);
    }

    return result;
}

void NodeConnection::removeAudioGraphConnection (uint32 sourceChannel, uint32 destChannel)
{
    AudioConnection ac = AudioConnection (sourceChannel, destChannel);

    if (AudioProcessorGraph* g = getParentGraph())
        g->removeConnection (AudioProcessorGraph::Connection(
                                                             {getAsNodeBase (sourceNode)->getAudioNode()->nodeID,
                                                            (int)sourceChannel},
                                                             {getAsNodeBase (destNode)->getAudioNode()->nodeID,
                                                            (int)destChannel}));


    model.audioConnections.removeAllInstancesOf (ac);
    listeners.call (&Listener::connectionAudioLinkRemoved, ac);


}
void NodeConnection::removeAllAudioGraphConnections()
{

    if (sourceNode.get() && destNode.get())
    {
        for (int i = 0 ; i < getAsNodeBase (sourceNode)->getTotalNumOutputChannels() ; i++)
        {
            for (int j = 0 ; j < getAsNodeBase (destNode)->getTotalNumInputChannels() ; j++)
            {
                removeAudioGraphConnection (i, j);
            }
        }
    }

    //    for(auto c:audioConnections){
    //        listeners.call(&Listener::connectionAudioLinkRemoved, c);
    //    }

    model.audioConnections.clear();

}

void NodeConnection::removeAllAudioGraphConnectionsForChannel (int channel, bool isSourceChannel)
{
    Array<AudioConnection> connectionsToRemove;

    for (auto c : model.audioConnections)
    {
        if ((isSourceChannel && c.first == channel) || (!isSourceChannel && c.second == channel))
        {
            connectionsToRemove.add (c);
        }
    }

    for (auto& tc : connectionsToRemove) removeAudioGraphConnection (tc.first, tc.second);


}


void NodeConnection::remove()
{
    listeners.call (&Listener::connectionRemoved, this);

    if (auto parent = sourceNode->getParentNodeContainer())
    {
        parent->removeConnection (this);
    }
}

void NodeConnection::audioInputAdded (ConnectableNode*, int )
{
    //DBG("Audio Input Added " << channel << " ( " << ghostConnections.size() << " ghosts )");

    Array<AudioConnection> connectionsToAdd;

    //DBG(" >> connections to add : " << connectionsToAdd.size());
    for (auto& c : connectionsToAdd)
    {
        addAudioGraphConnection (c.first, c.second);
    }
}

void NodeConnection::audioOutputAdded (ConnectableNode*, int )
{

    Array<AudioConnection> connectionsToAdd;

    for (auto& c : connectionsToAdd)
    {
        addAudioGraphConnection (c.first, c.second);
    }
}

void NodeConnection::audioInputRemoved (ConnectableNode* n, int channel)
{
    //DBG("Audio Input removed " << channel);
    if (n == destNode)
    {
        removeAllAudioGraphConnectionsForChannel (channel, false);
        //if (n->getTotalNumInputChannels() == 0) remove(); //not that useful with ghost support
    }
}

void NodeConnection::audioOutputRemoved (ConnectableNode* n, int channel)
{
    //DBG("Audio Output Removed "  << channel);
    if (n == sourceNode)
    {
        removeAllAudioGraphConnectionsForChannel (channel, true);
        //if (n->getTotalNumOutputChannels() == 0) remove(); //not useful with ghost support
    }
}


DynamicObject* NodeConnection::createObject()
{
    auto data = new DynamicObject();

    if (sourceNode.get() == nullptr || destNode.get() == nullptr)
    {
        DBG ("try to save outdated connection"); // TODO clean ghostConnection Nodes
        jassertfalse;
        return data;
    }

    ConnectableNode* tSource = sourceNode;

    if (auto s = dynamic_cast<ContainerOutNode*> (sourceNode.get())) tSource = (s)->getParentNodeContainer();

    ConnectableNode* tDest = destNode;

    if (auto s = dynamic_cast<ContainerInNode*> (destNode.get())) tDest = (s)->getParentNodeContainer();

    data->setProperty ("srcNode", tSource->shortName.toString());
    data->setProperty ("dstNode", tDest->shortName.toString());
    data->setProperty ("connectionType", (int)connectionType);

    var links;

    if (isAudio())
    {
        for (auto& c : model.audioConnections)
        {
            var cObject (new DynamicObject());
            cObject.getDynamicObject()->setProperty ("sourceChannel", c.first);
            cObject.getDynamicObject()->setProperty ("destChannel", c.second);
            links.append (cObject);
        }
    }

    data->setProperty ("links", links);

    return data;
}

void NodeConnection::configureFromObject (DynamicObject* data)
{
    //srcNodeId, destNodeId & connectionType set at creation, not in this load

    //DBG("Load JSON Data Node COnnection !");

    const Array<var>* links = data->getProperties().getWithDefault ("links", var()).getArray();

    if (links != nullptr)
    {
        if (isAudio())
        {
            removeAllAudioGraphConnections();

            for (const var& linkVar : *links)
            {
                int sourceChannel = linkVar.getProperty ("sourceChannel", var());
                int destChannel = linkVar.getProperty ("destChannel", var());
                //DBG("Add from JSON, " << sourceChannel << " > " <<destChannel);

                addAudioGraphConnection (sourceChannel, destChannel);
            }
        }

    }
}
