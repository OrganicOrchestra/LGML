/*
  ==============================================================================

    DummyNodeContentUI.cpp
    Created: 8 Mar 2016 7:20:14pm
    Author:  bkupe

  ==============================================================================
*/

#include "DummyNodeContentUI.h"

DummyNodeContentUI::DummyNodeContentUI() : NodeBaseContentUI()
{
}

DummyNodeContentUI::~DummyNodeContentUI()
{
}

void DummyNodeContentUI::resized()
{
	if (node == nullptr) return;
	Rectangle<int> r = getLocalBounds().reduced(50);
	testSlider->setBounds(r.removeFromTop(20));

	testTriggerButton->setTopRightPosition(getWidth()-10, getHeight() - testTriggerButton->getHeight() - 10);
}

void DummyNodeContentUI::init()
{
	//no need to call NodeBase init because there is nothing inside and there should not be anything !

	dummyNode = (DummyNode *)node;

	testSlider = dummyNode->testFloatParam->createSlider();
	addAndMakeVisible(testSlider);

	testTriggerButton = dummyNode->testTrigger->createButtonUI();
	addAndMakeVisible(testTriggerButton);

	nodeUI->setSize(200, 100);
}
