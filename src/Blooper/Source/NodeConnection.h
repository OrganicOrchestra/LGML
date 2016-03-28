/*
  ==============================================================================

    NodeConnection.h
    Created: 7 Mar 2016 12:39:02pm
    Author:  bkupe

  ==============================================================================
*/

#ifndef NODECONNECTION_H_INCLUDED
#define NODECONNECTION_H_INCLUDED

#include "JuceHeader.h"


#include "DataProcessorGraph.h"
class NodeManager;
class NodeBase;

class NodeConnection : public ReferenceCountedObject
{
public:
    enum ConnectionType
    {
        AUDIO, DATA, UNDEFINED
    };

    uint32 connectionId;

    ConnectionType connectionType;

    NodeManager * nodeManager;
    NodeBase * sourceNode;
    NodeBase * destNode;

    typedef std::pair<int,int> AudioConnection;
    Array<AudioConnection > audioConnections;
    Array<DataProcessorGraph::Connection *> dataConnections;


    NodeConnection(NodeManager* nodeManager,uint32 connectionId, NodeBase * sourceNode, NodeBase * destNode, ConnectionType connectionType);
    virtual ~NodeConnection();


    void addAudioGraphConnection(uint32 sourceChannel, uint32 destChannel);
    void removeAudioGraphConnection(uint32 sourceChannel, uint32 destChannel);
    void removeAllAudioGraphConnections();

    void addDataGraphConnection(const String &sourceDataName, const String &sourceElementName,const String &destDataName, const String &destElementName);
    void removeDataGraphConnection(const String &sourceDataName, const String &sourceElementName,const String &destDataName, const String &destElementName);

    void remove();

    //Listener
    class  Listener
    {
    public:
        /** Destructor. */
        virtual ~Listener() {}

        virtual void connectionEdited(NodeConnection *) = 0;
        virtual void askForRemoveConnection(NodeConnection *) = 0;
    };

    ListenerList<Listener> listeners;
    void addConnectionListener(Listener* newListener) { listeners.add(newListener); }
    void removeConnectionListener(Listener* listener) { listeners.remove(listener); }


    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(NodeConnection)

};


#endif  // NODECONNECTION_H_INCLUDED
