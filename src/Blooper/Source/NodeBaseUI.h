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
#include "ConnectorComponent.h"

#include "BoolToggleUI.h"

class NodeBase;
class NodeManagerUI;

//==============================================================================
/*

NodeBaseUI provide UI for blocks seen in NodeManagerUI
*/
class NodeBaseUI    : public Component
{
public:
	NodeBaseUI(NodeBase * node);

    virtual ~NodeBaseUI();
	
	NodeBase * node;
	virtual void setNode(NodeBase * node);
	
    void paint (Graphics&);
    void resized();
	
	//layout
	int connectorWidth;

	//ui
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
		ScopedPointer<BoolToggleUI> enabledToggle;

		MainContainer() :ContourComponent(Colours::green) 
		{
			headerHeight = 30;
			
			addAndMakeVisible(headerContainer);
			addAndMakeVisible(contentContainer);

			titleLabel.setColour(Label::ColourIds::textColourId,TEXT_COLOR);
			titleLabel.setJustificationType(Justification::topLeft);
			titleLabel.setInterceptsMouseClicks(false, false);
			headerContainer.addAndMakeVisible(titleLabel);
		}

		void setUIFromNode(NodeBase * node)
		{
			titleLabel.setText(node->name, NotificationType::dontSendNotification);
			enabledToggle = node->enabledParam->createToggle();
			headerContainer.addAndMakeVisible(enabledToggle);

		}

		void paint(Graphics &g)
		{

			//g.fillAll(PANEL_COLOR);   // clear the background
			g.setColour(PANEL_COLOR);
			g.fillRoundedRectangle(getLocalBounds().toFloat(), 4);
			g.setColour(CONTOUR_COLOR); 
			g.drawRoundedRectangle(getLocalBounds().toFloat(),4,2);
		}

		void resized()
		{
			Rectangle<int> r = getLocalBounds();
			Rectangle<int> headerBounds = r.removeFromTop(headerHeight);

			headerContainer.setBounds(headerBounds);
			contentContainer.setBounds(r);

			
			headerBounds.reduce(5, 2);
			headerBounds.removeFromLeft(enabledToggle->getWidth());

			titleLabel.setBounds(headerBounds);
			enabledToggle->setTopLeftPosition(5, 5);
		}
	};


	class ConnectorContainer : public ContourComponent
	{
	public:	
		OwnedArray<ConnectorComponent> connectors;

		ConnectorComponent::ConnectorDisplayLevel displayLevel;
		ConnectorComponent::ConnectorIOType type;

		ConnectorContainer(ConnectorComponent::ConnectorIOType type);

		void setConnectorsFromNode(NodeBase * node);
		void addConnector(ConnectorComponent::ConnectorIOType ioType, NodeConnection::ConnectionType dataType, NodeBase * node);
		void resized();

		ConnectorComponent * getFirstConnector(NodeConnection::ConnectionType dataType)
		{
			for (int i = 0; i < connectors.size(); i++)
			{
				if (connectors.getUnchecked(i)->dataType == dataType) return connectors.getUnchecked(i);
			}

			return nullptr;
		}

		JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(ConnectorContainer)
	};
    
    class VuMeter : public ContourComponent,public NodeBase::NodeAudioProcessor::Listener{
        
    public:
        VuMeter(){
            setSize(30,60);
        }
        
        void paint(Graphics &g)override{

            g.setColour(Colours::green);
            Rectangle<int> area = getLocalBounds();
            g.fillRect(area);
            g.setColour(Colours::black);
            g.fillRect(area.withHeight(area.getHeight()*(1.0-vol)));
            
        }
        float vol;
        void RMSChanged(float v) override {
            vol = v;
            repaint();
        };
        
    };
	
	MainContainer mainContainer;
	ConnectorContainer inputContainer;
	ConnectorContainer outputContainer;
    VuMeter vuMeter;

	Component * getContentContainer() { return &mainContainer.contentContainer; }
	Component * getHeaderContainer() { return &mainContainer.headerContainer; }

	Array<ConnectorComponent *> getComplementaryConnectors(ConnectorComponent * baseConnector)
	{
		Array<ConnectorComponent *> result;
		
		
		ConnectorContainer * checkSameCont = baseConnector->ioType == ConnectorComponent::ConnectorIOType::INPUT ? &inputContainer : &outputContainer;
		if (checkSameCont->getIndexOfChildComponent(baseConnector) != -1) return result;

		ConnectorContainer * complCont = checkSameCont == &inputContainer ? &outputContainer : &inputContainer;
		for (int i = 0; i < complCont->connectors.size(); i++)
		{
			ConnectorComponent *c = (ConnectorComponent *)complCont->getChildComponent(i);
			if (c->dataType == baseConnector->dataType)
			{
				result.add(c);
			}
		}

		return result;
	}
	NodeManagerUI * getNodeManagerUI() const noexcept;


	//Need to clean out and decide whether there can be more than 1 data connector / audio connector on nodes
	ConnectorComponent * getFirstConnector(NodeConnection::ConnectionType connectionType, ConnectorComponent::ConnectorIOType ioType)
	{
		if (ioType == ConnectorComponent::INPUT)
		{
			return inputContainer.getFirstConnector(connectionType);
		}
		else
		{
			return outputContainer.getFirstConnector(connectionType);
		}
	}

	Point<int> nodeInitPos;
	void mouseDown(const MouseEvent &e) override;
	void mouseDrag(const MouseEvent &e)  override;
	
private:
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (NodeBaseUI)
};


#endif  // NODEBASEUI_H_INCLUDED
