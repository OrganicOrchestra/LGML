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


#include "DummyNodeContentUI.h"
#include "../UI/ConnectableNodeUI.h"
#include "../../Controllable/Parameter/UI/ParameterUIFactory.h"
#if JUCE_DEBUG
DummyNodeContentUI::DummyNodeContentUI()
{
}

DummyNodeContentUI::~DummyNodeContentUI()
{
}

void DummyNodeContentUI::resized()
{
    if (node == nullptr) return;

    Rectangle<int> r = getLocalBounds().reduced (10);
    modeUI->setBounds (r.removeFromTop (20));
    r.removeFromTop (5);
    freq1Slider->setBounds (r.removeFromTop (25));
    r.removeFromTop (5);
    freq2Slider->setBounds (r.removeFromTop (25));
    r.removeFromTop (10);
    pxSlider->setBounds (r.removeFromTop (15));
    r.removeFromTop (5);
    pySlider->setBounds (r.removeFromTop (15));



    testTriggerButton->setTopRightPosition (getWidth() - 10, getHeight() - testTriggerButton->getHeight() - 10);
}

void DummyNodeContentUI::init()
{
    //no need to call NodeBase init because there is nothing inside and there should not be anything !

    dummyNode = (DummyNode*)node.get();

    freq1Slider = new FloatSliderUI (dummyNode->freq1Param);
    addAndMakeVisible (freq1Slider);

    freq2Slider = new FloatSliderUI (dummyNode->freq2Param);
    addAndMakeVisible (freq2Slider);

    pxSlider = new FloatSliderUI (dummyNode->pxParam);
    addAndMakeVisible (pxSlider);

    pySlider = new FloatSliderUI (dummyNode->pyParam);
    addAndMakeVisible (pySlider);

    testTriggerButton = ParameterUIFactory::createDefaultUI (dummyNode->testTrigger);
    addAndMakeVisible (testTriggerButton);

    modeUI = ParameterUIFactory::createDefaultUI (dummyNode->enumParam);
    addAndMakeVisible (modeUI);
    setDefaultSize (250, 200);


}
#endif // JUCE_DEBUG
