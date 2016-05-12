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

#include "InspectableComponent.h"

class NodeBase;
class NodeManagerUI;
#include "NodeBaseHeaderUI.h"
#include "NodeBaseContentUI.h"
#include "NodeBaseAudioCtlUI.h"

//==============================================================================
/*

NodeBaseUI provide UI for blocks seen in NodeManagerUI
*/
class NodeBaseUI    : 
	public InspectableComponent, 
	public Parameter::Listener
{
public:
    NodeBaseUI(NodeBase * node, NodeBaseContentUI * contentContainer = nullptr, NodeBaseHeaderUI * headerContainer = nullptr);
    virtual ~NodeBaseUI();

	//layout
	int connectorWidth;
	//interaction
	Point<int> nodeInitPos;


    NodeBase * node;
    virtual void setNode(NodeBase * node);
    // receives x y position from node parameters
    void paint (Graphics&)override;
	void paintOverChildren(Graphics &) override {} //cancel default yellow border behavior

	void resized()override;


    void childBoundsChanged (Component*)override;
	void parameterValueChanged(Parameter * p) override;
    
    void mouseDown(const MouseEvent &e) override;
    void mouseUp(const MouseEvent &e) override;
    void mouseDrag(const MouseEvent &e)  override;
	bool keyPressed(const KeyPress &key) override;
    void moved()override;

    //ui
    class MainContainer : public Component
    {
    public:

		//ui components
		MainContainer(NodeBaseUI * nodeUI, NodeBaseContentUI * content = nullptr, NodeBaseHeaderUI * header = nullptr);

        //reference
        NodeBaseUI * nodeUI;

        //containers
        ScopedPointer<NodeBaseHeaderUI> headerContainer;
        ScopedPointer<NodeBaseContentUI> contentContainer;
        ScopedPointer<NodeBaseAudioCtlUI> audioCtlUIContainer;

        const int audioCtlContainerPadRight = 3;
       
        void setNodeAndNodeUI(NodeBase * node, NodeBaseUI * nodeUI);
        void paint(Graphics &g) override;
        void resized() override;
        void childBoundsChanged (Component*)override;

		JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(MainContainer)
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
    bool dragIsLocked;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (NodeBaseUI)
};


#endif  // NODEBASEUI_H_INCLUDED
