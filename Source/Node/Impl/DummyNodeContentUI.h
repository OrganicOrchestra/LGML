/* Copyright © Organic Orchestra, 2017
*
* This file is part of LGML.  LGML is a software to manipulate sound in realtime
*
* This program is free software; you can redistribute it and/or modify
* it under the terms of the GNU General Public License as published by
* the Free Software Foundation (version 3 of the License).
*
* This program is distributed in the hope that it will be useful,
* but WITHOUT ANY WARRANTY; without even the implied warranty of
* MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
*
*/


#pragma once

#if JUCE_DEBUG

#include "DummyNode.h"//keep
#include "../UI/ConnectableNodeContentUI.h"
#include "../../Controllable/Parameter/UI/SliderUI.h"

#include "../../Controllable/Parameter/UI/EnumParameterUI.h"


class DummyNodeContentUI : public ConnectableNodeContentUI
{
public:
    DummyNodeContentUI();
    virtual ~DummyNodeContentUI();

    DummyNode* dummyNode;

    ScopedPointer<FloatSliderUI> freq1Slider;
    ScopedPointer<FloatSliderUI> freq2Slider;
    ScopedPointer<FloatSliderUI> pxSlider;
    ScopedPointer<FloatSliderUI> pySlider;
    ScopedPointer<ParameterUI> modeUI;
    ScopedPointer<ParameterUI> testTriggerButton;

    void resized() override;

    void init() override;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (DummyNodeContentUI)
};

#if NON_INCREMENTAL_COMPILATION
    #include "DummyNodeContentUI.cpp"
#endif

#endif // JUCE_DEBUG

