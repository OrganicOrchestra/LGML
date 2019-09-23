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

#include "../UI/ConnectableNodeContentUI.h"
#include "../../Controllable/Parameter/UI/ParameterUI.h"
#include "Spat2DNode.h"

class Spat2DViewer;

class Spat2DNodeContentUI :
    public ConnectableNodeContentUI,
    public Spat2DNode::ConnectableNodeListener
{
public:
    Spat2DNodeContentUI();
    virtual ~Spat2DNodeContentUI();

    Spat2DNode* spatNode;

    std::unique_ptr<Spat2DViewer> viewer2D;

    std::unique_ptr<ParameterUI> spatModeUI;
    std::unique_ptr<NamedParameterUI> inputStepper;
    std::unique_ptr<NamedParameterUI> outputStepper;
    std::unique_ptr<ParameterUI> radiusUI;
    std::unique_ptr<ParameterUI> useGlobalUI;
    std::unique_ptr<ParameterUI> globalRadiusUI;

    std::unique_ptr<ParameterUI> shapeModeUI;
    std::unique_ptr<ParameterUI> circleDiameterUI;
    std::unique_ptr<ParameterUI> circleRotationUI;

    void resized() override;
    void init() override;

    void updateShapeModeView();

    void nodeParameterChangedAsync (ConnectableNode*, ParameterBase* p) override;
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (Spat2DNodeContentUI)
};




#if NON_INCREMENTAL_COMPILATION
    #include "Spat2DNodeUI.cpp"
#endif
