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


#ifndef NODECONTAINERVIEWER_H_INCLUDED
#define NODECONTAINERVIEWER_H_INCLUDED

#include "../NodeContainer.h"
#include "../../UI/ConnectableNodeUI.h"

class NodeConnectionUI;

class NodeContainerViewer :
    public InspectableComponent,
    public NodeContainerListener,
    public ParameterContainer
{
public :
    NodeContainerViewer (NodeContainer* container,ParameterContainer * uiP);
    virtual ~NodeContainerViewer();

    NodeContainer* nodeContainer;
    

    
    OwnedArray<ConnectableNodeUI> nodesUI;
    OwnedArray<NodeConnectionUI>  connectionsUI;
    NodeConnectionUI* editingConnection;
    ScopedPointer<NodeConnection::Model> editingModel;
    void clear();

    void resized() override;

    // Inherited via NodeContainerListener
    virtual void nodeAdded (ConnectableNode*) override;
    virtual void nodeRemoved (ConnectableNode*) override;
    virtual void connectionAdded (NodeConnection*) override;
    virtual void connectionRemoved (NodeConnection*) override;

    void addNodeUI (ConnectableNode* node);
    void removeNodeUI (ConnectableNode* node);
    ConnectableNodeUI* getUIForNode (ConnectableNode* node);

    void addConnectionUI (NodeConnection* connection);
    void removeConnectionUI (NodeConnection* connection);
    NodeConnectionUI* getUIForConnection (NodeConnection* connection);

    //connection creation / editing
    typedef ConnectorComponent Connector;
    void createDataConnectionFromConnector (Connector* baseConnector);
    void createAudioConnectionFromConnector (Connector* baseConnector, NodeConnection* root = nullptr);

    void updateEditingConnection();
    bool isEditingConnection() { return editingConnection != nullptr; }
    bool checkDropCandidates();
    bool setCandidateDropConnector (Connector* c);
    void cancelCandidateDropConnector();
    void finishEditingConnection();


    //Mouse event
    void mouseDown (const MouseEvent& event) override;
    void mouseMove (const MouseEvent& event) override;
    void mouseDrag (const MouseEvent& event) override;
    void mouseUp (const MouseEvent& event) override;
    void childBoundsChanged (Component*)override;


    // key events
    bool keyPressed (const KeyPress& key)override;

    void resizeToFitNodes();

    ParameterContainer * uiParams;
    
};


#endif  // NODECONTAINERVIEWER_H_INCLUDED
