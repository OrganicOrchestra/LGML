/*
  ==============================================================================

    SpatNodeUI.cpp
    Created: 29 Sep 2016 7:20:25pm
    Author:  bkupe

  ==============================================================================
*/

#include "Spat2DNodeUI.h"
#include "NodeBaseUI.h"
#include "IntStepperUI.h"
#include "FloatSliderUI.h"

Spat2DNodeContentUI::Spat2DNodeContentUI() : NodeBaseContentUI()
{
}

Spat2DNodeContentUI::~Spat2DNodeContentUI()
{
	if (node != nullptr) node->removeConnectableNodeListener(this);
}


void Spat2DNodeContentUI::resized()
{
	if (node == nullptr) return;

	Rectangle<int> r = getLocalBounds().reduced(2);
	spatModeUI->setBounds(r.removeFromTop(20));
	r.removeFromTop(10);
	inputStepper->setBounds(r.removeFromTop(10));
	r.removeFromTop(2);
	outputStepper->setBounds(r.removeFromTop(10));
	r.removeFromTop(5);
	globalRadiusUI->setBounds(r.removeFromTop(15));
	r.removeFromTop(5);
	shapeModeUI->setBounds(r.removeFromTop(15));

	bool circleMode = (int)spatNode->shapeMode->getValueData() == Spat2DNode::ShapeMode::CIRCLE ;
	if (circleMode)
	{
		r.removeFromTop(5);
		circleRadiusUI->setBounds(r.removeFromTop(15));
		r.removeFromTop(2);
		circleRotationUI->setBounds(r.removeFromTop(15));
	}

	r.removeFromTop(5);

	
	if (viewer2D != nullptr)
	{
		Rectangle<int> vr = r.reduced(5);
		vr = vr.withSizeKeepingCentre(jmin<int>(vr.getWidth(), vr.getHeight()), jmin<int>(vr.getWidth(), vr.getHeight()));
		viewer2D->setBounds(vr);
	}

}
void Spat2DNodeContentUI::init()
{
	spatNode = (Spat2DNode *)node.get();
	spatNode->addConnectableNodeListener(this);
	

	viewer2D = new Spat2DViewer(spatNode);
	addAndMakeVisible(viewer2D);

	spatModeUI = spatNode->spatMode->createUI();
	addAndMakeVisible(spatModeUI);

	inputStepper = new NamedControllableUI(spatNode->numSpatInputs->createStepper(), 80);
	addAndMakeVisible(inputStepper);
	outputStepper = new NamedControllableUI(spatNode->numSpatOutputs->createStepper(), 80);
	addAndMakeVisible(outputStepper);

	globalRadiusUI = spatNode->globalTargetRadius->createSlider();
	addAndMakeVisible(globalRadiusUI);

	shapeModeUI = spatNode->shapeMode->createUI();
	addAndMakeVisible(shapeModeUI);

	circleRadiusUI = spatNode->circleRadius->createSlider();
	addChildComponent(circleRadiusUI);
	circleRotationUI = spatNode->circleRotation->createSlider();
	addChildComponent(circleRotationUI);

	updateShapeModeView();

	nodeUI->setSize(300, 400);
}

void Spat2DNodeContentUI::updateShapeModeView()
{
	bool circleMode = (int)spatNode->shapeMode->getValueData() == Spat2DNode::ShapeMode::CIRCLE;
	circleRadiusUI->setVisible(circleMode);
	circleRotationUI->setVisible(circleMode);
  resized();
}

void Spat2DNodeContentUI::nodeParameterChanged(ConnectableNode *, Parameter * p)
{
	if (p == spatNode->shapeMode) updateShapeModeView();

}
