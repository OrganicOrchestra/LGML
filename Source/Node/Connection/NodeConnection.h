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


#ifndef NODECONNECTION_H_INCLUDED
#define NODECONNECTION_H_INCLUDED
#pragma once


#include "../ConnectableNode.h"
#include "../../Utils/FactoryObject.h"


class NodeConnection :
    public ReferenceCountedObject,
    public ConnectableNode::ConnectableNodeListener,
    public FactoryObject
{
public:
    enum ConnectionType
    {
        AUDIO, UNDEFINED
    };

    typedef std::pair<int, int> AudioConnection;
    class Model
    {
        // keeps all connection info (used to keep info after deletion)
    public:
        Array<AudioConnection> audioConnections;

    };
    Model model;

    NodeConnection (ConnectableNode* sourceNode, ConnectableNode* destNode, ConnectionType connectionType, Model* root = nullptr);

    virtual ~NodeConnection();

    DECLARE_OBJ_TYPE (NodeConnection,"connect nodes together")
    ConnectionType connectionType;

    bool isAudio() { return connectionType == ConnectionType::AUDIO; }
    

    WeakReference<ConnectableNode> sourceNode;
    WeakReference<ConnectableNode> destNode;


    //Audio
    bool addAudioGraphConnection (uint32 sourceChannel, uint32 destChannel);
    void removeAudioGraphConnection (uint32 sourceChannel, uint32 destChannel);
    void removeAllAudioGraphConnections();

    void removeAllAudioGraphConnectionsForChannel (int channel, bool isSourceChannel);


    void remove();

    virtual void audioInputAdded (ConnectableNode*, int /*channel*/) override;
    virtual void audioOutputAdded (ConnectableNode*, int /*channel*/) override;

    virtual void audioInputRemoved (ConnectableNode*, int /* channel */) override;
    virtual void audioOutputRemoved (ConnectableNode*, int /* channel */) override;




    // save / load

    DynamicObject* getObject()override;
    void configureFromObject (DynamicObject* data) override;

    //Listener
    class  Listener
    {
    public:
        /** Destructor. */
        virtual ~Listener() {}

        virtual void connectionRemoved (NodeConnection*) {}

        virtual void connectionAudioLinkAdded (const AudioConnection&) {}
        virtual void connectionAudioLinkRemoved (const AudioConnection&) {}
    };

    ListenerList<Listener> listeners;
    void addConnectionListener (Listener* newListener) { jassert (newListener); listeners.add (newListener); }
    void removeConnectionListener (Listener* listener) { listeners.remove (listener); }

    AudioProcessorGraph* getParentGraph();

private:
    WeakReference<NodeConnection >::Master masterReference;
    friend class WeakReference<NodeConnection>;



    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (NodeConnection)

};


#endif  // NODECONNECTION_H_INCLUDED
