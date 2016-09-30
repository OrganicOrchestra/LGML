/*
  ==============================================================================

    SpatNodeUI.cpp
    Created: 29 Sep 2016 7:20:25pm
    Author:  bkupe

  ==============================================================================
*/

#include "SpatNodeUI.h"
#include "NodeBaseUI.h"
#include "IntStepperUI.h"

SpatNodeContentUI::SpatNodeContentUI() : NodeBaseContentUI()
{
}

SpatNodeContentUI::~SpatNodeContentUI()
{
}

void SpatNodeContentUI::updateSpatModeView()
{
	DBG("udpate spat mode view :" << spatNode->spatMode->getValueData().toString());

	if (viewer3D != nullptr) removeChildComponent(viewer3D);
	if (viewer2D != nullptr) removeChildComponent(viewer2D);

	if (spatNode->modeIs2D())
	{
		viewer3D = nullptr;
		viewer2D = new Spat2DViewer(spatNode);
		addAndMakeVisible(viewer2D);
	} else
	{
		viewer2D = nullptr;
		viewer3D = new Spat3DViewer();
		addAndMakeVisible(viewer3D);
	}

	if (viewer2D != nullptr) viewer2D->setNumSources(spatNode->numSpatInputs->intValue());
	else if (viewer3D != nullptr) viewer3D->setNumSources(spatNode->numSpatInputs->intValue());
	if (viewer2D != nullptr) viewer2D->setNumTargets(spatNode->numSpatOutputs->intValue());
	else if (viewer3D != nullptr) viewer3D->setNumTargets(spatNode->numSpatOutputs->intValue());
}

void SpatNodeContentUI::resized()
{
	if (node == nullptr) return;

	Rectangle<int> r = getLocalBounds().reduced(2);
	spatModeUI->setBounds(r.removeFromTop(20));
	r.removeFromTop(10);
	
	Rectangle<int> ioR = r.removeFromTop(20);
	inputStepper->setBounds(ioR.removeFromLeft(ioR.getWidth()/2 - 5));
	ioR.removeFromLeft(10);
	outputStepper->setBounds(ioR);

	r.removeFromTop(5);

	
	if (viewer2D != nullptr)
	{
		viewer2D->setBounds(r);
	} else if(viewer3D != nullptr)
	{
		viewer3D->setBounds(r);
	}

}
void SpatNodeContentUI::init()
{
	spatNode = (SpatNode *)node.get();

	spatModeUI = spatNode->spatMode->createUI();
	addAndMakeVisible(spatModeUI);

	inputStepper = new NamedControllableUI(spatNode->numSpatInputs->createStepper(), 30);
	addAndMakeVisible(inputStepper);
	outputStepper = new NamedControllableUI(spatNode->numSpatOutputs->createStepper(), 30);
	addAndMakeVisible(outputStepper);

	updateSpatModeView();

	nodeUI->setSize(250, 300);
}

void SpatNodeContentUI::modeChanged()
{
	updateSpatModeView();
}

void SpatNodeContentUI::numSpatInputsChanged()
{
	if (viewer2D != nullptr) viewer2D->setNumSources(spatNode->numSpatInputs->intValue());
	else if(viewer3D != nullptr) viewer3D->setNumSources(spatNode->numSpatInputs->intValue());
}

void SpatNodeContentUI::numSpatOutputsChanged()
{
	if (viewer2D != nullptr) viewer2D->setNumTargets(spatNode->numSpatOutputs->intValue());
	else if (viewer3D != nullptr) viewer3D->setNumTargets(spatNode->numSpatOutputs->intValue());
}
