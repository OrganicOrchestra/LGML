/*
  ==============================================================================

    DummyNodeContentUI.h
    Created: 8 Mar 2016 7:20:14pm
    Author:  bkupe

  ==============================================================================
*/

#ifndef DUMMYNODECONTENTUI_H_INCLUDED
#define DUMMYNODECONTENTUI_H_INCLUDED


#include "DummyNode.h"//keep
#include "NodeBaseContentUI.h"
#include "FloatSliderUI.h"

#include "EnumParameterUI.h"

class DummyNodeContentUI : public NodeBaseContentUI
{
public:
    DummyNodeContentUI();
    virtual ~DummyNodeContentUI();

    DummyNode * dummyNode;

    ScopedPointer<FloatSliderUI> freq1Slider;
    ScopedPointer<FloatSliderUI> freq2Slider;
	ScopedPointer<FloatSliderUI> pxSlider;
	ScopedPointer<FloatSliderUI> pySlider;
	ScopedPointer<EnumParameterUI> modeUI;
    ScopedPointer<ControllableUI> testTriggerButton;

    void resized() override;

    void init() override;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(DummyNodeContentUI)
};


#endif  // DUMMYNODECONTENTUI_H_INCLUDED
