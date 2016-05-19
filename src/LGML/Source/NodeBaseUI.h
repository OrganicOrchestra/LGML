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
#include "ConnectableNodeUI.h"
#include "NodeBaseHeaderUI.h"
#include "NodeBaseContentUI.h"
#include "NodeBaseAudioCtlUI.h"

class NodeBase;

//==============================================================================
/*

NodeBaseUI provide UI for blocks seen in NodeManagerUI
*/
class NodeBaseUI    :
	public ConnectableNodeUI,
	public Parameter::Listener
{
public:
    NodeBaseUI(NodeBase * node, NodeBaseContentUI * contentContainer = nullptr, NodeBaseHeaderUI * headerContainer = nullptr);
    virtual ~NodeBaseUI();

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


    MainContainer mainContainer;
    
    NodeBaseContentUI * getContentContainer() { return mainContainer.contentContainer; }
    NodeBaseHeaderUI * getHeaderContainer() { return mainContainer.headerContainer; }

    

   
private:
    bool dragIsLocked;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (NodeBaseUI)
};


#endif  // NODEBASEUI_H_INCLUDED
