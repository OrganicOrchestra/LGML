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

#include "ConnectorComponent.h"
#include "../NodeContainer/UI/NodeContainerViewer.h"
#include "../Manager/NodeFactory.h"
#include "../../UI/Style.h"
#include "../NodeBase.h"

ConnectorComponent::ConnectorComponent (ConnectorIOType _ioType, NodeConnection::ConnectionType _dataType, ConnectableNode* _cnode) :
    ioType (_ioType),
    dataType (_dataType),
    node ((ConnectableNode*)_cnode),
    isHovered (false)
{


    node->addConnectableNodeListener (this);

    boxColor =  findColour ((dataType == NodeConnection::ConnectionType::AUDIO) ? LGMLColors::audioColor : LGMLColors::dataColor);
    setSize (10, 10);


    postCommandMessage (0);
}

ConnectorComponent::~ConnectorComponent()
{
    if (node.get())
        node->removeConnectableNodeListener (this);
    else
    {
        jassertfalse;
    }
}

NodeBase* ConnectorComponent::getNodeBase()
{
    return dynamic_cast<NodeBase*> (node.get());
}

void ConnectorComponent::generateToolTip()
{
    String tooltip;
    tooltip += dataType == NodeConnection::ConnectionType::AUDIO ? "Audio\n" : "Data\n";

    if (dataType == NodeConnection::ConnectionType::AUDIO)
    {
        bool isInput = ioType == ConnectorIOType::INPUT;
        NodeBase* tAudioNode = ((NodeBase*)node.get());

        if (tAudioNode != nullptr)
        {
            tooltip += isInput ? tAudioNode->getTotalNumInputChannels() : tAudioNode->getTotalNumOutputChannels();
            tooltip += " channels";
        }
        else
        {
            tooltip = "[Error accessing audio processor]";
        }
    }
    else
    {

        StringArray dataInfos = ioType == ConnectorIOType::INPUT ? node->getInputDataInfos() : node->getOutputDataInfos();
        tooltip += dataInfos.joinIntoString ("\n");

    }

    setTooltip (tooltip);
}
void ConnectorComponent::paint (Graphics& g)
{
    g.setGradientFill (ColourGradient (isHovered ? boxColor.brighter (5.f) : boxColor, (float) (getLocalBounds().getCentreY()), (float) (getLocalBounds().getCentreY()), boxColor.darker(), 0.f, 0.f, true));
    g.fillRoundedRectangle (getLocalBounds().toFloat(), 2);
}

void ConnectorComponent::mouseDown (const MouseEvent&)
{
    NodeContainerViewer* containerViewer = getNodeContainerViewer();

    if (dataType == NodeConnection::ConnectionType::DATA)
    {

        containerViewer->createDataConnectionFromConnector (this);
    }
    else
    {
        containerViewer->createAudioConnectionFromConnector (this);
    }
}

void ConnectorComponent::mouseEnter (const MouseEvent&)
{
    isHovered = true;
    repaint();
}
void ConnectorComponent::mouseExit  (const MouseEvent&)
{
    isHovered = false;
    repaint();
}

void ConnectorComponent::updateVisibility()
{

    bool isAudio = dataType == NodeConnection::ConnectionType::AUDIO;
    bool isInput = ioType == ConnectorIOType::INPUT;

    if (isAudio) setVisible (isInput ? node->hasAudioInputs() : node->hasAudioOutputs());
    else setVisible (isInput ? node->hasDataInputs() : node->hasDataOutputs());

    connectorListeners.call (&ConnectorListener::connectorVisibilityChanged, this);
}

void ConnectorComponent::numAudioInputChanged (ConnectableNode*, int)
{
    if (dataType != NodeConnection::ConnectionType::AUDIO || ioType != ConnectorIOType::INPUT) return;

    postCommandMessage (0);
}

void ConnectorComponent::numAudioOutputChanged (ConnectableNode*, int)
{
    if (dataType != NodeConnection::ConnectionType::AUDIO || ioType != ConnectorIOType::OUTPUT) return;

    postCommandMessage (0);
}
void ConnectorComponent::handleCommandMessage (int /*id*/)
{
    updateVisibility();

}

//DATA
void ConnectorComponent::numDataInputChanged (ConnectableNode*, int)
{
    if (dataType != NodeConnection::ConnectionType::DATA || ioType != ConnectorIOType::INPUT) return;

    postCommandMessage (0);
}

void ConnectorComponent::numDataOutputChanged (ConnectableNode*, int)
{
    if (dataType != NodeConnection::ConnectionType::DATA || ioType != ConnectorIOType::OUTPUT) return;

    postCommandMessage (0);
}


NodeContainerViewer* ConnectorComponent::getNodeContainerViewer() const noexcept
{
    return findParentComponentOfClass<NodeContainerViewer>();
}

ConnectableNodeUI* ConnectorComponent::getNodeUI() const noexcept
{
    return findParentComponentOfClass<ConnectableNodeUI>();
}
