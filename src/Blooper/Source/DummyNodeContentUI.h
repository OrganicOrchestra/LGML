/*
  ==============================================================================

    DummyNodeContentUI.h
    Created: 8 Mar 2016 7:20:14pm
    Author:  bkupe

  ==============================================================================
*/

#ifndef DUMMYNODECONTENTUI_H_INCLUDED
#define DUMMYNODECONTENTUI_H_INCLUDED

#include "NodeBaseContentUI.h"
#include "FloatSliderUI.h"

class DummyNodeContentUI : public NodeBaseContentUI
{
public:
	DummyNodeContentUI();
	virtual ~DummyNodeContentUI();

	NodeBaseUI * nodeUI;
	NodeBase * node;

	DummyNode * dummyNode;

	ScopedPointer<FloatSliderUI> testSlider;

	void resized() override;

	void setNodeAndNodeUI(NodeBase * node, NodeBaseUI * nodeUI) override;

	JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(DummyNodeContentUI)
};


#endif  // DUMMYNODECONTENTUI_H_INCLUDED
