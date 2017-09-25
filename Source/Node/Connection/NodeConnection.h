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

#include "../../Data/DataProcessorGraph.h"
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
    AUDIO, DATA, UNDEFINED
  };

  typedef std::pair<int,int> AudioConnection;
  class Model{
    // keeps all connection info (used to keep info after deletion)
  public:
    Array<AudioConnection> audioConnections;
    Array<DataProcessorGraph::Connection *> dataConnections;
  };
  Model model;

  NodeConnection(ConnectableNode * sourceNode, ConnectableNode * destNode, ConnectionType connectionType,Model * root = nullptr);
  NodeConnection getCopy();
  virtual ~NodeConnection();

  DECLARE_OBJ_TYPE(NodeConnection)
  ConnectionType connectionType;

  bool isAudio() { return connectionType == ConnectionType::AUDIO; }
  bool isData() { return connectionType == ConnectionType::DATA; }

  WeakReference<ConnectableNode> sourceNode;
  WeakReference<ConnectableNode> destNode;

  
  //Audio
  bool addAudioGraphConnection(uint32 sourceChannel, uint32 destChannel);
  void removeAudioGraphConnection(uint32 sourceChannel, uint32 destChannel);
  void removeAllAudioGraphConnections();

  void removeAllAudioGraphConnectionsForChannel(int channel, bool isSourceChannel);

  //Data
  void addDataGraphConnection(Data * sourceData, Data * destData);
  void removeDataGraphConnection(Data * sourceData, Data * destData);
  void removeAllDataGraphConnections();

  void removeAllDataGraphConnectionsForData(Data *, bool isSourceData);

  void remove();

  virtual void audioInputAdded(ConnectableNode *, int /*channel*/) override;
  virtual void audioOutputAdded(ConnectableNode *, int /*channel*/) override;

  virtual void audioInputRemoved(ConnectableNode *, int /* channel */) override;
  virtual void audioOutputRemoved(ConnectableNode *, int /* channel */) override;


  virtual void dataInputRemoved(ConnectableNode *, Data *) override;
  virtual void dataOutputRemoved(ConnectableNode *, Data *) override;


  // save / load

  DynamicObject * getObject()override;
  void configureFromObject(DynamicObject * data) override;

  //Listener
  class  Listener
  {
  public:
    /** Destructor. */
    virtual ~Listener() {}

    virtual void connectionRemoved(NodeConnection *) {}

    virtual void connectionDataLinkAdded(DataProcessorGraph::Connection * ) {}
    virtual void connectionDataLinkRemoved(DataProcessorGraph::Connection * ) {}

    virtual void connectionAudioLinkAdded(const AudioConnection &) {}
    virtual void connectionAudioLinkRemoved(const AudioConnection &) {}
  };

  ListenerList<Listener> listeners;
  void addConnectionListener(Listener* newListener) { jassert(newListener);listeners.add(newListener); }
  void removeConnectionListener(Listener* listener) { listeners.remove(listener); }

  AudioProcessorGraph * getParentGraph();

protected:
  WeakReference<NodeConnection >::Master masterReference;
  friend class WeakReference<NodeConnection>;



  JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(NodeConnection)

};


#endif  // NODECONNECTION_H_INCLUDED
