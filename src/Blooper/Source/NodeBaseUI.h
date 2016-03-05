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
#include "UIHelpers.h"

class NodeBase;
class NodeManagerUI;


//==============================================================================
/*

NodeBaseUI provide UI for blocks seen in NodeManagerUI
*/
class NodeBaseUI    : public Component
{
public:
	NodeBaseUI();

    virtual ~NodeBaseUI();
	
	NodeBase * node;
	virtual void setNode(NodeBase * node);
	
    void paint (Graphics&);
    void resized();
	
	//layout
	int connectorWidth;
	
	class MainContainer : public ContourComponent
	{
	public:
		//layout
		int headerHeight;

		//containers
		ContourComponent headerContainer;
		ContourComponent contentContainer;

		//ui components
		Label titleLabel;

		MainContainer() :ContourComponent(Colours::green) 
		{
			headerHeight = 30;
			
			addAndMakeVisible(headerContainer);
			addAndMakeVisible(contentContainer);

			titleLabel.setColour(Label::ColourIds::textColourId,TEXT_COLOR);
			headerContainer.addAndMakeVisible(titleLabel);
		}

		void paint(Graphics &g)
		{
			g.fillAll(PANEL_COLOR);   // clear the background
			g.setColour(CONTOUR_COLOR);
			g.drawRect(getLocalBounds());
		}

		void resized()
		{
			Rectangle<int> r = getLocalBounds();
			Rectangle<int> headerBounds = r.removeFromTop(headerHeight);

			headerContainer.setBounds(headerBounds);
			contentContainer.setBounds(r);

			titleLabel.setBounds(headerBounds);
		}
	};


	class ConnectorContainer : public ContourComponent
	{
	public:
		
		class ConnectorComponent : public Component
		{
		public:

			enum ConnectorIOType
			{
				INPUT, OUTPUT
			};

			enum ConnectorDataType
			{
				AUDIO, DATA
			};

			enum ConnectorDisplayLevel
			{
				MINIMAL, CONNECTED, ALL
			};


			ConnectorDataType dataType;
			ConnectorIOType ioType;
			NodeBase * node;

			//layout

			//style
			Colour boxColor;

			ConnectorComponent(ConnectorIOType ioType, ConnectorDataType dataType, NodeBase * node);
			
			void paint(Graphics &g)
			{
				g.fillAll(boxColor);
			}

			void mouseDown(const MouseEvent &e) override;
			void mouseDrag(const MouseEvent &e) override;
			void mouseUp(const MouseEvent &e) override;

			
			NodeManagerUI * getNodeManagerUI() const noexcept;
			NodeBaseUI * getNodeUI() const noexcept;

			JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(ConnectorComponent)
		};

	
		OwnedArray<ConnectorComponent> connectors;

		ConnectorComponent::ConnectorDisplayLevel displayLevel;
		ConnectorComponent::ConnectorIOType type;

		ConnectorContainer(ConnectorComponent::ConnectorIOType type);

		void setConnectorsFromNode(NodeBase * node);
		void addConnector(ConnectorComponent::ConnectorIOType ioType, ConnectorComponent::ConnectorDataType dataType, NodeBase * node);
		void resized();

		JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(ConnectorContainer)
	};
	
	MainContainer mainContainer;
	ConnectorContainer inputContainer;
	ConnectorContainer outputContainer;

	Component * getContentContainer() { return &mainContainer.contentContainer; }
	Component * getHeaderContainer() { return &mainContainer.headerContainer; }

	Array<ConnectorContainer::ConnectorComponent *> getComplementaryConnectors(ConnectorContainer::ConnectorComponent * baseConnector)
	{
		Array<ConnectorContainer::ConnectorComponent *> result;
		
		
		ConnectorContainer * checkSameCont = baseConnector->ioType == ConnectorContainer::ConnectorComponent::ConnectorIOType::INPUT ? &inputContainer : &outputContainer;
		if (checkSameCont->getIndexOfChildComponent(baseConnector) != -1) return result;

		ConnectorContainer * complCont = checkSameCont == &inputContainer ? &outputContainer : &inputContainer;
		for (int i = 0; i < complCont->connectors.size(); i++)
		{
			ConnectorContainer::ConnectorComponent *c = (ConnectorContainer::ConnectorComponent *)complCont->getChildComponent(i);
			if (c->dataType == baseConnector->dataType)
			{
				result.add(c);
			}
		}

		return result;
	}
	NodeManagerUI * getNodeManagerUI() const noexcept;

	Point<int> nodeInitPos;
	void mouseDown(const MouseEvent &e) override;
	void mouseDrag(const MouseEvent &e)  override;
	
private:
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (NodeBaseUI)
};


#endif  // NODEBASEUI_H_INCLUDED
