/*
  ==============================================================================

    DummyNodeContentUI.cpp
    Created: 8 Mar 2016 7:20:14pm
    Author:  bkupe

  ==============================================================================
*/

#include "DummyNodeContentUI.h"
#include "NodeBaseUI.h"

DummyNodeContentUI::DummyNodeContentUI() : NodeBaseContentUI()
{
}

DummyNodeContentUI::~DummyNodeContentUI()
{
}

void DummyNodeContentUI::resized()
{
    if (node == nullptr) return;
    Rectangle<int> r = getLocalBounds().reduced(10);
	modeUI->setBounds(r.removeFromTop(20));
	r.removeFromTop(10);
	freq1Slider->setBounds(r.removeFromTop(25));
    r.removeFromTop(10);
    freq2Slider->setBounds(r.removeFromTop(25));

	

    testTriggerButton->setTopRightPosition(getWidth()-10, getHeight() - testTriggerButton->getHeight() - 10);
}

void DummyNodeContentUI::init()
{
    //no need to call NodeBase init because there is nothing inside and there should not be anything !

    dummyNode = (DummyNode *)node.get();

    freq1Slider = dummyNode->freq1Param->createSlider();
    addAndMakeVisible(freq1Slider);

    freq2Slider = dummyNode->freq2Param->createSlider();
    addAndMakeVisible(freq2Slider);

    testTriggerButton = dummyNode->testTrigger->createButtonUI();
    addAndMakeVisible(testTriggerButton);

	modeUI = dummyNode->enumParam->createUI();
	addAndMakeVisible(modeUI);

	nodeUI->setSize(250, 150);
}
