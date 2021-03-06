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

#if !ENGINE_HEADLESS && !NON_INCREMENTAL_COMPILATION

#include "Spat2DViewerUI.hpp"
#include "Spat2DNodeUI.h"
#include "../UI/ConnectableNodeUI.h"
#include "../../Controllable/Parameter/UI/StepperUI.h"
#include "../../Controllable/Parameter/UI/SliderUI.h"
#include "../../Controllable/Parameter/UI/ParameterUIFactory.h"

Spat2DNodeContentUI::Spat2DNodeContentUI()
{
}

Spat2DNodeContentUI::~Spat2DNodeContentUI()
{
    if (node != nullptr) node->removeConnectableNodeListener (this);
}


void Spat2DNodeContentUI::resized()
{
    if (node == nullptr) return;

    Rectangle<int> r = getLocalBounds().reduced (2);
    spatModeUI->setBounds (r.removeFromTop (20));
    r.removeFromTop (10);
    inputStepper->setBounds (r.removeFromTop (10));
    r.removeFromTop (2);
    outputStepper->setBounds (r.removeFromTop (10));
    r.removeFromTop (5);
    radiusUI->setBounds (r.removeFromTop (15));
    r.removeFromTop (5);
    useGlobalUI->setBounds (r.removeFromTop (15));
    r.removeFromTop (5);

    if (spatNode->useGlobalTarget->boolValue())
    {
        globalRadiusUI->setBounds (r.removeFromTop (15));
        r.removeFromTop (5);
    }

    if(shapeModeUI)
        shapeModeUI->setBounds (r.removeFromTop (15));

    bool circleMode = (int)spatNode->shapeMode->getFirstSelectedValue() == Spat2DNode::ShapeMode::CIRCLE ;

    if (circleMode)
    {
        r.removeFromTop (5);
        circleDiameterUI->setBounds (r.removeFromTop (15));
        r.removeFromTop (2);
        circleRotationUI->setBounds (r.removeFromTop (15));
    }

    r.removeFromTop (5);


    if (viewer2D != nullptr)
    {
        Rectangle<int> vr = r.reduced (5);
        vr = vr.withSizeKeepingCentre (jmin<int> (vr.getWidth(), vr.getHeight()), jmin<int> (vr.getWidth(), vr.getHeight()));
        viewer2D->setBounds (vr);
    }

}
void Spat2DNodeContentUI::init()
{
    spatNode = (Spat2DNode*)node.get();
    spatNode->addConnectableNodeListener (this);


    viewer2D = std::make_unique< Spat2DViewer> (spatNode);
    addAndMakeVisible (viewer2D.get());

    spatModeUI = ParameterUIFactory::createDefaultUI (spatNode->spatMode);
    addAndMakeVisible (spatModeUI.get());

    inputStepper = std::make_unique< NamedParameterUI> (ParameterUIFactory::createDefaultUI (spatNode->numSpatInputs), 80);
    addAndMakeVisible (inputStepper.get());
    outputStepper = std::make_unique< NamedParameterUI> (ParameterUIFactory::createDefaultUI (spatNode->numSpatOutputs), 80);
    addAndMakeVisible (outputStepper.get());

    radiusUI = std::make_unique< FloatSliderUI> (spatNode->targetRadius);
    addAndMakeVisible (radiusUI.get());

    useGlobalUI = ParameterUIFactory::createDefaultUI (spatNode->useGlobalTarget);
    addAndMakeVisible (useGlobalUI.get());

    if (spatNode->useGlobalTarget->boolValue() && globalRadiusUI != nullptr)
    {
        globalRadiusUI = std::make_unique< FloatSliderUI> (spatNode->globalTargetRadius);
        addAndMakeVisible (globalRadiusUI.get());
    }

    shapeModeUI =nullptr;
//    shapeModeUI = ParameterUIFactory::createDefaultUI (spatNode->shapeMode);
//    addAndMakeVisible (shapeModeUI);

    circleDiameterUI = std::make_unique< FloatSliderUI> (spatNode->circleDiameter);
    addChildComponent (circleDiameterUI.get());

    circleRotationUI = std::make_unique< FloatSliderUI> (spatNode->circleRotation);
    addChildComponent (circleRotationUI.get());

    updateShapeModeView();
    setDefaultSize (300, 400);


}

void Spat2DNodeContentUI::updateShapeModeView()
{
    bool circleMode = (int)spatNode->shapeMode->getFirstSelectedValue() == Spat2DNode::ShapeMode::CIRCLE;
    circleDiameterUI->setVisible (circleMode);
    circleRotationUI->setVisible (circleMode);
    resized();
}

void Spat2DNodeContentUI::nodeParameterChangedAsync (ConnectableNode*, ParameterBase* p)
{
    if (p == spatNode->shapeMode) updateShapeModeView();
    else if (p == spatNode->useGlobalTarget)
    {
        if (spatNode->useGlobalTarget->boolValue())
        {
            globalRadiusUI = std::make_unique< FloatSliderUI> (spatNode->globalTargetRadius);
            addAndMakeVisible (globalRadiusUI.get());

        }
        else
        {
            removeChildComponent (globalRadiusUI.get());
            globalRadiusUI = nullptr;
        }

        resized();
    }
}

#endif
