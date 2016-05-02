/*
  ==============================================================================

    NodeBaseUI.h
    Created: 3 Mar 2016 11:52:50pm
    Author:  bkupe

  ==============================================================================
*/

#ifndef NODEBASEUI_H_INCLUDED
#define NODEBASEUI_H_INCLUDED

#include "Style.h"
#include "ConnectorComponent.h"

#include "NodeBaseHeaderUI.h"
#include "NodeBaseContentUI.h"
#include "SelectableComponent.h"

class NodeBase;
class NodeManagerUI;
class NodeBaseHeaderUI;
class NodeBaseContentUI;

//==============================================================================
/*

NodeBaseUI provide UI for blocks seen in NodeManagerUI
*/
class NodeBaseUI    : public SelectableComponent, public Parameter::Listener
{
public:
    NodeBaseUI(NodeBase * node, NodeBaseContentUI * contentContainer = nullptr, NodeBaseHeaderUI * headerContainer = nullptr);
    virtual ~NodeBaseUI();

    NodeBase * node;
    virtual void setNode(NodeBase * node);
    // receives x y position from node parameters
    void parameterValueChanged(Parameter * p) override;
    void paint (Graphics&)override;
    void resized()override;
    void childBoundsChanged (Component*)override;

    //layout
    int connectorWidth;

    //interaction
    Point<int> nodeInitPos;
    void mouseDown(const MouseEvent &e) override;
    void mouseUp(const MouseEvent &e) override;
    void mouseDrag(const MouseEvent &e)  override;
    void moved()override;


    //SelectableComponent
    void internalSetSelected(bool _selected)override;
    void paintOverChildren(Graphics &) override {} //cancel default yellow border behavior

    //ui
    class MainContainer : public Component
    {
    public:

        //reference
        NodeBaseUI * nodeUI;

        //containers
        ScopedPointer<NodeBaseHeaderUI> headerContainer;
        ScopedPointer<NodeBaseContentUI> contentContainer;

        //ui components
        MainContainer(NodeBaseUI * nodeUI, NodeBaseContentUI * content = nullptr, NodeBaseHeaderUI * header = nullptr);
        void setNodeAndNodeUI(NodeBase * node, NodeBaseUI * nodeUI);
        void paint(Graphics &g) override;
        void resized() override;
        void childBoundsChanged (Component*)override;
    };


    class ConnectorContainer : public Component
    {
    public:

        OwnedArray<ConnectorComponent> connectors;

        ConnectorComponent::ConnectorDisplayLevel displayLevel;
        ConnectorComponent::ConnectorIOType type;

        ConnectorContainer(ConnectorComponent::ConnectorIOType type);

        void setConnectorsFromNode(NodeBase * node);
        void addConnector(ConnectorComponent::ConnectorIOType ioType, NodeConnection::ConnectionType dataType, NodeBase * node);
        void resized();

        ConnectorComponent * getFirstConnector(NodeConnection::ConnectionType dataType);

        JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(ConnectorContainer)
    };


    MainContainer mainContainer;
    ConnectorContainer inputContainer;
    ConnectorContainer outputContainer;

    NodeBaseContentUI * getContentContainer() { return mainContainer.contentContainer; }
    NodeBaseHeaderUI * getHeaderContainer() { return mainContainer.headerContainer; }

    Array<ConnectorComponent *> getComplementaryConnectors(ConnectorComponent * baseConnector);
    NodeManagerUI * getNodeManagerUI() const noexcept;


    //Need to clean out and decide whether there can be more than 1 data connector / audio connector on nodes
    ConnectorComponent * getFirstConnector(NodeConnection::ConnectionType connectionType, ConnectorComponent::ConnectorIOType ioType);



private:


    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (NodeBaseUI)
};


#endif  // NODEBASEUI_H_INCLUDED
