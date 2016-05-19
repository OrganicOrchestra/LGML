/*
==============================================================================

ConnectorComponent.h
Created: 7 Mar 2016 11:48:35am
Author:  bkupe

==============================================================================
*/

#ifndef CONNECTORCOMPONENT_H_INCLUDED
#define CONNECTORCOMPONENT_H_INCLUDED

#include "NodeConnection.h"

class NodeBase;
class NodeBaseUI;
class NodeContainerViewer;

//==============================================================================
/*
*/
class ConnectorComponent : 
	public Component, 
	public SettableTooltipClient,
	public NodeBase::NodeAudioProcessorListener
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
    NodeBase * node;

    //layout

    //style
    Colour boxColor;
    bool isHovered;

    ConnectorComponent(ConnectorIOType ioType, NodeConnection::ConnectionType dataType, NodeBase * node);
    ~ConnectorComponent(); 
    void paint(Graphics &g)override;

    void mouseDown(const MouseEvent &e) override;
    void mouseEnter (const MouseEvent&)override;
    void mouseExit  (const MouseEvent&)override;


    //void selectDataAndElementPopup(String &selectedDataName, String &selectedElementName, DataType &selectedDataType, const DataType &filterType = DataType::Unknown);

    NodeContainerViewer * getNodeContainerViewer() const noexcept;
    ConnectableNodeUI * getNodeUI() const noexcept;

    void numAudioInputChanged(int newNum)override;
    void numAudioOutputChanged(int newNum)override;



private:
    void generateToolTip();

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(ConnectorComponent)
};


#endif  // CONNECTORCOMPONENT_H_INCLUDED
