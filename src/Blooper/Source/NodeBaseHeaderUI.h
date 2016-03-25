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

class NodeBaseHeaderUI : public ContourComponent
{
public:
	NodeBaseHeaderUI();
	virtual ~NodeBaseHeaderUI();

	NodeBase * node;
	NodeBaseUI * nodeUI;

	ScopedPointer<StringParameterUI> titleUI;
	ScopedPointer<BoolToggleUI> enabledUI;
	VuMeter vuMeter;

    
    void mouseDoubleClick(const MouseEvent & e)override;

    
	virtual void setNodeAndNodeUI(NodeBase * node, NodeBaseUI * nodeUI);
	virtual void init();


	virtual void resized() override;

	JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(NodeBaseHeaderUI)
};




#endif  // NODEBASEHEADERUI_H_INCLUDED
