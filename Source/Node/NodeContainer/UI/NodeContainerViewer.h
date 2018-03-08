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

typedef WeakReference<ConnectableNodeUI> SelectedUIType;

class NodeConnectionUI;
class ParameterUI;
class NodeContainerViewer :
    public InspectableComponent,
    public NodeContainerListener,
    public ParameterContainer,
public ChangeListener, // multiselection
private LassoSource<SelectedUIType>
{
public :
    NodeContainerViewer (NodeContainer* container,ParameterContainer * uiP);
    virtual ~NodeContainerViewer();

    NodeContainer* nodeContainer;
    

    BoolParameter * minimizeAll;
    
    OwnedArray<ConnectableNodeUI> nodesUI;
    OwnedArray<NodeConnectionUI>  connectionsUI;
    NodeConnectionUI* editingConnection;
    ScopedPointer<NodeConnection::Model> editingModel;
    void clear();

    void resized() override;
    void onContainerParameterChanged(Parameter * p) override;


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


    SelectedItemSet<SelectedUIType> selectedItems;

    void addNodeUndoable(const String & type,const Point<int> & mousePos);
    String getTooltip() override{return String::empty;}
private:

    void changeListenerCallback (ChangeBroadcaster* source) override;
    LassoComponent<SelectedUIType> lassoSelectionComponent;
    Component nodesLayer;
    bool resultOfMouseDownSelectMethod,hasDraggedDuringClick;
    void findLassoItemsInArea (Array<SelectedUIType>& itemsFound,
                               const Rectangle<int>& area) override;

    SelectedItemSet<SelectedUIType>& getLassoSelection() override;
    HashMap<SelectedUIType, Rectangle<int>> selectedInitBounds;






    
};


#endif  // NODECONTAINERVIEWER_H_INCLUDED
