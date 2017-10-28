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


#ifndef CONNECTABLENODEUI_H_INCLUDED
#define CONNECTABLENODEUI_H_INCLUDED
#pragma once

#include "../../UI/Inspector/InspectableComponent.h"
#include "../ConnectableNode.h"
#include "ConnectorComponent.h"

class ConnectableNodeHeaderUI;
#include "ConnectableNodeContentUI.h"
#include "ConnectableNodeAudioCtlUI.h"


class ConnectableNodeUIParams: public ParameterContainer{
public:
    ConnectableNodeUIParams(ConnectableNodeUIParams * _origin);
    ConnectableNodeUIParams(StringRef n);
    void initFromParams();
    void notifyFromParams();
    virtual ~ConnectableNodeUIParams();
    Point2DParameter<int>* getCurrentPositionParam();

    

    Point2DParameter<int>* nodePosition;
    Point2DParameter<int>* nodeMinimizedPosition;
    Point2DParameter<int>* nodeSize;
    BoolParameter* miniMode;
    WeakReference<ParameterContainer>  origin;

};

class ConnectableNodeUI :
    public InspectableComponent,
    public ConnectableNode::ConnectableNodeListener,
    public ConnectableNodeUIParams
{
public:
    ConnectableNodeUI (ConnectableNode* cn, ConnectableNodeUIParams* params ,ConnectableNodeContentUI* contentUI = nullptr, ConnectableNodeHeaderUI* headerUI = nullptr);
    virtual ~ConnectableNodeUI();


    WeakReference<ConnectableNode> connectableNode;


    ResizableCornerComponent resizer;
    ComponentBoundsConstrainer constrainer;

    class ConnectorContainer :
        public juce::Component,
        public ConnectorComponent::ConnectorListener
    {
    public:
        OwnedArray<ConnectorComponent> connectors;

        ConnectorComponent::ConnectorDisplayLevel displayLevel;
        ConnectorComponent::ConnectorIOType type;

        ConnectorContainer (ConnectorComponent::ConnectorIOType type);

        void setConnectorsFromNode (ConnectableNode* node);
        void addConnector (ConnectorComponent::ConnectorIOType ioType, NodeConnection::ConnectionType dataType, ConnectableNode* node);
        void resized() override;

        void connectorVisibilityChanged (ConnectorComponent*) override;


        ConnectorComponent* getFirstConnector (NodeConnection::ConnectionType dataType);



        JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (ConnectorContainer)
    };

    //ui
    class MainComponentContainer : public juce::Component
    {
    public:

        //ui y
        MainComponentContainer (ConnectableNodeUI* nodeUI, ConnectableNodeContentUI* content = nullptr, ConnectableNodeHeaderUI* header = nullptr);

        //reference
        ConnectableNodeUI* connectableNodeUI;

        //containers
        ScopedPointer<ConnectableNodeHeaderUI> headerContainer;
        ScopedPointer<ConnectableNodeContentUI> contentContainer;
        ScopedPointer<ConnectableNodeAudioCtlUI> audioCtlUIContainer;

        static constexpr int audioCtlContainerPadRight = 3;
        static constexpr int audioCtlContainerWidth = 10;

        void setNodeAndNodeUI (ConnectableNode* node, ConnectableNodeUI* nodeUI);
        void paint (Graphics& g) override;
        void resized() override;

        bool miniMode;
        virtual void setMiniMode (bool value);

        void childBoundsChanged (Component*)override;

        JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (MainComponentContainer)
    };





    // @ben conflit avec le parametre ;)
    bool bMiniMode;


//    DynamicObject* getObject();
//    void setFromObject(const DynamicObject * obj);




    MainComponentContainer mainComponentContainer;
    ConnectableNodeContentUI* getContentContainer() { return mainComponentContainer.contentContainer; }
    ConnectableNodeHeaderUI* getHeaderContainer() { return mainComponentContainer.headerContainer; }

    ConnectorContainer inputContainer;
    ConnectorContainer outputContainer;


    // receives x y position from node parameters
    void paint (Graphics&)override;
    void paintOverChildren (Graphics&) override {} //cancel default yellow border behavior

    void resized()override;


    virtual void setMiniMode (bool value);
    virtual int getMiniModeWidth (bool forMiniMode);
    virtual int getMiniModeHeight (bool forMiniMode);

    Array<ConnectorComponent*> getComplementaryConnectors (ConnectorComponent* baseConnector);

    //Need to clean out and decide whether there can be more than 1 data connector / audio connector on nodes
    ConnectorComponent* getFirstConnector (NodeConnection::ConnectionType connectionType, ConnectorComponent::ConnectorIOType ioType);

private:

    //interaction
    Point<int> nodeInitPos;

    void childBoundsChanged (Component*)override;
    void nodeParameterChanged (ConnectableNode*, Parameter* p) override;
    void onContainerParameterChanged(Parameter *p) override;

    void mouseDown (const MouseEvent& e) override;
    void mouseUp (const MouseEvent& e) override;
    void mouseDrag (const MouseEvent& e)  override;
    bool keyPressed (const KeyPress& key) override;
    void moved()override;

    void postOrHandleCommandMessage (int id);

    void handleCommandMessage (int id) override;
    enum
    {
        repaintId,
        setMiniModeId,
        posChangedId,
        sizeChangedId
    } CommandMessages;

    // avoid laggy self param updates
    bool isDraggingFromUI;
};


#endif  // CONNECTABLENODEUI_H_INCLUDED
