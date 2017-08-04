/*
  ==============================================================================

    ConnectableNodeUI.h
    Created: 18 May 2016 11:34:23pm
    Author:  bkupe

  ==============================================================================
*/

#ifndef CONNECTABLENODEUI_H_INCLUDED
#define CONNECTABLENODEUI_H_INCLUDED
#pragma once

#include "InspectableComponent.h"
#include "ConnectableNode.h"
#include "ConnectorComponent.h"

class ConnectableNodeHeaderUI;
#include "ConnectableNodeContentUI.h"
#include "ConnectableNodeAudioCtlUI.h"


class ConnectableNodeUI :
	public InspectableComponent,
	public ConnectableNode::ConnectableNodeListener
{
public:
	ConnectableNodeUI(ConnectableNode * cn, ConnectableNodeContentUI * contentUI = nullptr, ConnectableNodeHeaderUI * headerUI = nullptr);
	virtual ~ConnectableNodeUI();

	WeakReference<ConnectableNode> connectableNode;

	
	ResizableCornerComponent resizer;
  ComponentBoundsConstrainer constrainer;

	class ConnectorContainer :
		public Component,
		public ConnectorComponent::ConnectorListener
	{
	public:
		OwnedArray<ConnectorComponent> connectors;

		ConnectorComponent::ConnectorDisplayLevel displayLevel;
		ConnectorComponent::ConnectorIOType type;

		ConnectorContainer(ConnectorComponent::ConnectorIOType type);

		void setConnectorsFromNode(ConnectableNode * node);
		void addConnector(ConnectorComponent::ConnectorIOType ioType, NodeConnection::ConnectionType dataType, ConnectableNode * node);
		void resized() override;

		void connectorVisibilityChanged(ConnectorComponent *) override;


		ConnectorComponent * getFirstConnector(NodeConnection::ConnectionType dataType);
    


		JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(ConnectorContainer)
	};

	//ui
	class MainComponentContainer : public Component
	{
	public:

		//ui y
		MainComponentContainer(ConnectableNodeUI * nodeUI, ConnectableNodeContentUI * content = nullptr, ConnectableNodeHeaderUI * header = nullptr);

		//reference
		ConnectableNodeUI * connectableNodeUI;

		//containers
		ScopedPointer<ConnectableNodeHeaderUI> headerContainer;
		ScopedPointer<ConnectableNodeContentUI> contentContainer;
		ScopedPointer<ConnectableNodeAudioCtlUI> audioCtlUIContainer;

		const int audioCtlContainerPadRight = 3;
    const int audioCtlContainerWidth=10;

		void setNodeAndNodeUI(ConnectableNode * node, ConnectableNodeUI * nodeUI);
		void paint(Graphics &g) override;
		void resized() override;

		bool miniMode;
		virtual void setMiniMode(bool value);

		void childBoundsChanged(Component*)override;

		JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(MainComponentContainer)
	};



	//layout
	int connectorWidth;

  // @ben conflit avec le parametre ;)
	bool bMiniMode;

	//interaction
	Point<int> nodeInitPos;
	bool dragIsLocked;




	MainComponentContainer mainComponentContainer;
	ConnectableNodeContentUI * getContentContainer() { return mainComponentContainer.contentContainer; }
	ConnectableNodeHeaderUI * getHeaderContainer() { return mainComponentContainer.headerContainer; }

	ConnectorContainer inputContainer;
	ConnectorContainer outputContainer;


	// receives x y position from node parameters
	void paint(Graphics&)override;
	void paintOverChildren(Graphics &) override {} //cancel default yellow border behavior

	void resized()override;


	virtual void setMiniMode(bool value);
	virtual int getMiniModeWidth(bool forMiniMode);
	virtual int getMiniModeHeight(bool forMiniMode);

	Array<ConnectorComponent *> getComplementaryConnectors(ConnectorComponent * baseConnector);

	//Need to clean out and decide whether there can be more than 1 data connector / audio connector on nodes
	ConnectorComponent * getFirstConnector(NodeConnection::ConnectionType connectionType, ConnectorComponent::ConnectorIOType ioType);

private:
	void childBoundsChanged(Component*)override;
	void nodeParameterChanged(ConnectableNode*, Parameter * p) override;


	void mouseDown(const MouseEvent &e) override;
	void mouseUp(const MouseEvent &e) override;
	void mouseDrag(const MouseEvent &e)  override;
	bool keyPressed(const KeyPress &key) override;
	void moved()override;


void handleCommandMessage(int id) override;
enum{
  repaintId  ,
  setMiniModeId,
  posChangedId,
  sizeChangedId
}CommandMessages;

  // avoid laggy self param updates
  bool isDraggingFromUI;
};


#endif  // CONNECTABLENODEUI_H_INCLUDED
