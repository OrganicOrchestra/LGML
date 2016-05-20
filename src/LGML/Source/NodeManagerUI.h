/*
 ==============================================================================

 NodeManagerUI.h
 Created: 3 Mar 2016 10:38:22pm
 Author:  bkupe

 ==============================================================================
 */

#ifndef NODEMANAGERUI_H_INCLUDED
#define NODEMANAGERUI_H_INCLUDED


#include "ShapeShifterContent.h"
#include "NodeContainerViewer.h"


//==============================================================================
/*
 Draw all connected Nodes and Connections
 */
class NodeManagerUI : 
	public Component
{
public:
	
    NodeManagerUI(NodeManager * nodeManager);
    ~NodeManagerUI();

    NodeManager * nodeManager;
	ScopedPointer<NodeContainerViewer> currentViewer;

    void clear();

    void resized() override;

	int getContentWidth();
	int getContentHeight();

	void setCurrentViewedContainer(NodeContainer * c);
    
	void childBoundsChanged(Component * c);

	class  NodeManagerUIListener
	{
	public:
		/** Destructor. */
		virtual ~NodeManagerUIListener() {}
		virtual void currentViewedContainerChanged() {};
	};

	ListenerList<NodeManagerUIListener> nodeManagerUIListeners;
	void addNodeManagerUIListener(NodeManagerUIListener* newListener) { nodeManagerUIListeners.add(newListener); }
	void removeNodeManagerUIListener(NodeManagerUIListener* listener) { nodeManagerUIListeners.remove(listener); }


    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (NodeManagerUI)
	
};

class NodeManagerUIViewport :
	public ShapeShifterContent,
	public NodeManagerUI::NodeManagerUIListener,
	public ButtonListener
{
public :
    NodeManagerUIViewport(const String &contentName, NodeManagerUI * _nmui):nmui(_nmui),ShapeShifterContent(contentName)
	{
        vp.setViewedComponent(nmui,true);
        vp.setScrollBarsShown(true, true);
        vp.setScrollOnDragEnabled(false);
        contentIsFlexible = true;
        addAndMakeVisible(vp);
		vp.setScrollBarThickness(10);

		nmui->addNodeManagerUIListener(this);

		reconstructViewerPath();

    }

	virtual ~NodeManagerUIViewport()
	{
		nmui->removeNodeManagerUIListener(this);

	}
	
	OwnedArray<TextButton> pathButtons;

	void reconstructViewerPath()
	{
		for (auto &b : pathButtons)
		{
			removeChildComponent(b);
			b->removeListener(this);
		}

		pathButtons.clear();

		NodeContainer * c = nmui->currentViewer->nodeContainer;

		while (c != nullptr)
		{
			DBG("Add path button " << c->niceName);
			TextButton * b = new TextButton(c->niceName);
			if (c == nmui->currentViewer->nodeContainer) b->setEnabled(false);
			
			pathButtons.insert(0, b);
			addAndMakeVisible(b); 
			b->addListener(this); 
			
			c = c->parentNodeContainer;
			
		}


		resized();
	}

    void resized() override{
		Rectangle<int> r = getLocalBounds();

		Rectangle<int> buttonR = r.removeFromTop(30);
				
		for (auto & b : pathButtons)
		{
			b->setBounds(buttonR.removeFromLeft(100));
			buttonR.removeFromLeft(5);
		}

		r.removeFromTop(2);

		vp.setBounds(r);
		nmui->setBounds(r);
		nmui->setSize(nmui->getContentWidth() == 0?getWidth():nmui->getContentWidth(),
					  nmui->getContentHeight() == 0?getHeight():nmui->getContentHeight());

    }

	void currentViewedContainerChanged()
	{
		DBG("Reconstruct");
		reconstructViewerPath();
	}

	void buttonClicked(Button * b)
	{
		int bIndex = pathButtons.indexOf((TextButton *)b);
		if (bIndex == -1)
		{
			DBG("WTF ?");
		}
		NodeContainer * c = nmui->currentViewer->nodeContainer;
		for (int i = pathButtons.size() - 1; i > bIndex; i--)
		{
			c = c->parentNodeContainer;
		}

		nmui->setCurrentViewedContainer(c);

	}

    Viewport vp;
    NodeManagerUI * nmui;
};

#endif  // NODEMANAGERUI_H_INCLUDED
