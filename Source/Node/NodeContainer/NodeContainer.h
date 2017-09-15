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


#ifndef NODECONTAINER_H_INCLUDED
#define NODECONTAINER_H_INCLUDED
#pragma once

#include "../Manager/NodeFactory.h"
#include "../ConnectableNode.h"
#include "../Connection/NodeConnection.h"

#include "../Impl/ContainerInNode.h"
#include "../Impl/ContainerOutNode.h"
#include "../../Controllable/Parameter/ParameterProxy.h"


// WIP, will need to go deep if we want to take avantage of a multi Threaded environnement
//#define MULTITHREADED_AUDIO

class NodeManager;


//Listener

class NodeChangeMessage{
public:
  NodeChangeMessage(ConnectableNode *n,bool isAd):node(n),connection(nullptr),isAdded(isAd){}
  NodeChangeMessage(NodeConnection *con,bool isAd):node(nullptr),connection(con),isAdded(isAd){}

  ConnectableNode * node = nullptr;
  NodeConnection * connection = nullptr;
  bool isAdded;

};

typedef  QueuedNotifier<NodeChangeMessage> NodeChangeQueue;


class  NodeContainerListener : public NodeChangeQueue::Listener
{
public:
  /** Destructor. */
  virtual ~NodeContainerListener() {}

  virtual void nodeAdded(ConnectableNode *) {};
  virtual void nodeRemoved(ConnectableNode *) {};

  virtual void connectionAdded(NodeConnection *) {};
  virtual void connectionRemoved(NodeConnection *) {};

  virtual void paramProxyAdded(ParameterProxy *) {};
  virtual void paramProxyRemoved(ParameterProxy *) {};

   void newMessage(const NodeChangeMessage & msg )override{
     if(msg.node){
       if(msg.isAdded)nodeAdded(msg.node);
       else nodeRemoved(msg.node);
     }
     if(msg.connection){
       if(msg.isAdded)connectionAdded(msg.connection);
       else connectionRemoved(msg.connection);
     }
  }

};


class NodeContainer :
public NodeBase,
public ConnectableNode::ConnectableNodeListener,
public NodeConnection::Listener,
public ConnectableNode::RMSListener,
public ParameterProxy::ParameterProxyListener,
public AsyncUpdater

{
public:
  DECLARE_OBJ_TYPE(NodeContainer);
  virtual ~NodeContainer();

  //Keep value of containerIn RMS and containerOutRMS to dispatch in one time
  float rmsInValue;
  float rmsOutValue;


  //Container nodes, not removable by user, handled separately
  ContainerInNode * containerInNode;
  ContainerOutNode * containerOutNode;
  ScopedPointer<AudioProcessorGraph> innerGraph;
  AudioProcessorGraph * getAudioGraph(){return innerGraph;};


  //NODE AND CONNECTION MANAGEMENT

  Array<WeakReference<ConnectableNode> > nodes; //Not OwnedArray anymore because NodeBase is AudioProcessor, therefore owned by AudioProcessorGraph
  OwnedArray<NodeConnection> connections;
  Array<NodeContainer*> nodeContainers; //so they are delete on "RemoveNode" (because they don't have an audio processor)



  
  ConnectableNode* addNodeFromJSONData(DynamicObject * data);
  ConnectableNode* addNode(ConnectableNode * node,const String &nodeName = String::empty, DynamicObject * data = nullptr);
  
  bool removeNode(ConnectableNode * n);

  ConnectableNode * getNodeForName(const String &name);

  NodeConnection * getConnection(const int index) const noexcept { return connections[index]; }
  NodeConnection * getConnectionBetweenNodes(ConnectableNode * sourceNode, ConnectableNode * destNode, NodeConnection::ConnectionType connectionType);
  Array<NodeConnection *> getAllConnectionsForNode(ConnectableNode * node);

  NodeConnection * addConnection(ConnectableNode * sourceNode, ConnectableNode * destNode, NodeConnection::ConnectionType connectionType,NodeConnection::Model * root=nullptr);
  bool removeConnection(NodeConnection * c);
  void removeIllegalConnections();
  int getNumConnections();

  int getNumNodes() const noexcept { return nodes.size(); }

  // called to bypass this container
  void bypassNode(bool bypass);

  ParameterProxy * addParamProxy();
  void removeParamProxy(ParameterProxy * pp);

  //save / load
  DynamicObject * getObject() override;
  void configureFromObject(DynamicObject * data) override;
  ParameterContainer *  addContainerFromObject(const String & name,DynamicObject *  v)override;
  
  void clear()override;
  void clear(bool keepContainerNodes);

  // Inherited via NodeBase::Listener
  virtual void askForRemoveNode(ConnectableNode *) override;


  // Inherited via NodeConnection::Listener
  virtual void askForRemoveConnection(NodeConnection *) override;

  // Inherited via RMSListener
  virtual void RMSChanged(ConnectableNode * node, float rmsInValue, float rmsOutValue) override;


  virtual void onContainerParameterChanged(Parameter * p) override;
  virtual void onContainerParameterChangedAsync(Parameter * p ,const var & value)override;

  

  //AUDIO

  void updateAudioGraph(bool lock = true) ;
  void numChannelsChanged(bool isInput) override;
  
  //DATA
  bool hasDataInputs() override;
  bool hasDataOutputs() override;

  void processBlockInternal(AudioBuffer<float>& buffer , MidiBuffer& midiMessage ) override;
  void processBlockBypassed(AudioBuffer<float>& buffer, MidiBuffer& midiMessages)override;

  virtual void prepareToPlay(double d, int i) override ;
  virtual void releaseResources() override {
    NodeBase::releaseResources();
    getAudioGraph()->releaseResources();};



  ListenerList<NodeContainerListener> nodeContainerListeners;
  void addNodeContainerListener(NodeContainerListener* newListener) { nodeContainerListeners.add(newListener);nodeChangeNotifier.addListener(newListener); }
  void removeNodeContainerListener(NodeContainerListener* listener) { nodeContainerListeners.remove(listener);nodeChangeNotifier.removeListener(listener); }


#ifdef MULTITHREADED_AUDIO
  class GraphJob :public ThreadPoolJob{
  public:
    GraphJob(AudioProcessorGraph* _graph,String name):
    ThreadPoolJob("GraphJob : "+name),graph(_graph)
    {

    }

    void setBuffersToReferTo(AudioBuffer<float> &buffer,MidiBuffer & midiMessage){
      buf = &buffer;
      midiMess = &midiMessage;
    }

    JobStatus runJob()override{
      graph->processBlock(*buf,*midiMess);
      return JobStatus::jobHasFinished;
    };


    AudioBuffer<float> * buf;
    MidiBuffer * midiMess;
    AudioProcessorGraph * graph;
  };
  ScopedPointer<GraphJob> graphJob;
  NodeManager * nodeManager;
#endif

  void handleAsyncUpdate()override;
  
  class RebuildTimer : public Timer{
  public:
    RebuildTimer(NodeContainer* o):owner(o){};
    void timerCallback()override {
      owner->triggerAsyncUpdate();
    };
    NodeContainer * owner;
    
  };
  RebuildTimer rebuildTimer;
  NodeChangeQueue nodeChangeNotifier;

  JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(NodeContainer)
  
  
  
};


#endif  // NODECONTAINER_H_INCLUDED
