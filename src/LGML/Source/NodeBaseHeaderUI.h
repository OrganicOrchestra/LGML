/*
  ==============================================================================

    NodeBaseHeaderUI.h
    Created: 8 Mar 2016 5:53:52pm
    Author:  bkupe

  ==============================================================================
*/

#ifndef NODEBASEHEADERUI_H_INCLUDED
#define NODEBASEHEADERUI_H_INCLUDED

#include "JuceHeader.h"
#include "UIHelpers.h"
#include "NodeBaseUI.h"

#include "StringParameterUI.h"
#include "BoolToggleUI.h"
#include "VuMeter.h"
#include "Style.h"



class NodeBaseHeaderUI : public ContourComponent
{
public:

	class Grabber : public Component
	{
		void paint(Graphics &g) override
		{
			g.setColour(FRONT_COLOR);
			Rectangle<int> r = getLocalBounds().reduced(5);
			g.drawLine(r.getTopLeft().x, r.getRelativePoint(0, .2f).y, r.getTopRight().x, r.getRelativePoint(0, .2f).y, .4f);
			g.drawLine(r.getTopLeft().x, r.getCentreY() - .5f, r.getBottomRight().x, r.getCentreY() - .5f, .4f);
			g.drawLine(r.getBottomLeft().x, r.getRelativePoint(0, .8f).y, r.getBottomRight().x, r.getRelativePoint(0, .8f).y, .4f);

		}
	};

	NodeBaseHeaderUI();
    virtual ~NodeBaseHeaderUI();

    NodeBase * node;
    NodeBaseUI * nodeUI;

    ScopedPointer<StringParameterUI> titleUI;
    ScopedPointer<BoolToggleUI> enabledUI;
    VuMeter vuMeter;

	Grabber grabber;
	ImageButton removeBT;

    void mouseDoubleClick(const MouseEvent & e)override;


    virtual void setNodeAndNodeUI(NodeBase * node, NodeBaseUI * nodeUI);
    virtual void init();


    virtual void resized() override;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(NodeBaseHeaderUI)
};




#endif  // NODEBASEHEADERUI_H_INCLUDED
