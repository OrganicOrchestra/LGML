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


#ifndef CONNECTORCOMPONENT_H_INCLUDED
#define CONNECTORCOMPONENT_H_INCLUDED

#include "../Connection/NodeConnection.h"

class ConnectableNode;
class ConnectableNodeUI;
class NodeContainerViewer;

//==============================================================================
/*
*/
class ConnectorComponent :
    public juce::Component,
    public SettableTooltipClient,
    public ConnectableNode::ConnectableNodeListener
{
public:

    enum ConnectorIOType
    {
        INPUT, OUTPUT
    };


    enum ConnectorDisplayLevel
    {
        MINIMAL, CONNECTED, ALL
    };


    NodeConnection::ConnectionType dataType;
    ConnectorIOType ioType;
    WeakReference<ConnectableNode> node;
    NodeBase* getNodeBase();

    //layout

    //style
    Colour boxColor;
    bool isHovered;

    ConnectorComponent (ConnectorIOType ioType, NodeConnection::ConnectionType dataType, ConnectableNode* cnode);
    ~ConnectorComponent();

    void paint (Graphics& g)override;

    void mouseDown (const MouseEvent& e) override;
    void mouseEnter (const MouseEvent&)override;
    void mouseExit  (const MouseEvent&)override;

    void updateVisibility();


    NodeContainerViewer* getNodeContainerViewer() const noexcept;
    ConnectableNodeUI* getNodeUI() const noexcept;

    void numAudioInputChanged (ConnectableNode*, int newNum) override;
    void numAudioOutputChanged (ConnectableNode*, int newNum) override;

    void handleCommandMessage (int id)override;


    //Listener
    class  ConnectorListener
    {
    public:
        /** Destructor. */
        virtual ~ConnectorListener() {}

        virtual void connectorVisibilityChanged (ConnectorComponent*) {};
    };

    ListenerList<ConnectorListener> connectorListeners;
    void addConnectorListener (ConnectorListener* newListener) { connectorListeners.add (newListener); }
    void removeConnectorListener (ConnectorListener* listener) { connectorListeners.remove (listener); }
private:
    void generateToolTip();

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (ConnectorComponent)
};


#endif  // CONNECTORCOMPONENT_H_INCLUDED
