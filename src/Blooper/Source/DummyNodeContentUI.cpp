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
	testSlider->setBounds(getLocalBounds().reduced(50).removeFromTop(50));
}

void DummyNodeContentUI::setNodeAndNodeUI(NodeBase * node, NodeBaseUI * nodeUI)
{
	NodeBaseContentUI::setNodeAndNodeUI(node, nodeUI);
	dummyNode = (DummyNode *)node;

	testSlider = dummyNode->testFloatParam->createSlider();
	addAndMakeVisible(testSlider);
}
