/*
  ==============================================================================

    DummyNodeContentUI.cpp
    Created: 8 Mar 2016 7:20:14pm
    Author:  bkupe

  ==============================================================================
*/

#include "DummyNodeContentUI.h"
#include "NodeBaseUI.h"
#include "ParameterUIFactory.h"

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
	r.removeFromTop(5);
	freq1Slider->setBounds(r.removeFromTop(25));
    r.removeFromTop(5);
    freq2Slider->setBounds(r.removeFromTop(25));
	r.removeFromTop(10);
	pxSlider->setBounds(r.removeFromTop(15));
	r.removeFromTop(5);
	pySlider->setBounds(r.removeFromTop(15));

	

    testTriggerButton->setTopRightPosition(getWidth()-10, getHeight() - testTriggerButton->getHeight() - 10);
}

void DummyNodeContentUI::init()
{
    //no need to call NodeBase init because there is nothing inside and there should not be anything !

    dummyNode = (DummyNode *)node.get();

    freq1Slider = new FloatSliderUI(dummyNode->freq1Param);
    addAndMakeVisible(freq1Slider);

    freq2Slider = new FloatSliderUI(dummyNode->freq2Param);
    addAndMakeVisible(freq2Slider);

	pxSlider = new FloatSliderUI(dummyNode->pxParam);
	addAndMakeVisible(pxSlider);

	pySlider = new FloatSliderUI(dummyNode->pyParam);
	addAndMakeVisible(pySlider);

    testTriggerButton = ParameterUIFactory::createDefaultUI(dummyNode->testTrigger);
    addAndMakeVisible(testTriggerButton);

	modeUI = ParameterUIFactory::createDefaultUI(dummyNode->enumParam);
	addAndMakeVisible(modeUI);
  setDefaultSize(250,200);


}
