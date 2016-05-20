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

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (NodeManagerUI)
	
};

class NodeManagerUIViewport: public ShapeShifterContent
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

    }
    void resized() override{
        vp.setSize(getWidth(), getHeight());
		nmui->setBounds(getLocalBounds());

		nmui->setSize(nmui->getContentWidth() == 0?getWidth():nmui->getContentWidth(),
					  nmui->getContentHeight() == 0?getHeight():nmui->getContentHeight());

    }
    Viewport vp;
    NodeManagerUI * nmui;
};

#endif  // NODEMANAGERUI_H_INCLUDED
