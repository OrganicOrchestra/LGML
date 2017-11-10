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


#ifndef VSTNODEUI_H_INCLUDED
#define VSTNODEUI_H_INCLUDED



#include "../../Controllable/Parameter/UI/SliderUI.h"
#include "../../MIDI/MIDIUIHelper.h"
#include "VSTNode.h"
#include "../UI/ConnectableNodeContentUI.h"


class VSTNodeContentUI:
    public ConnectableNodeContentUI,
    public Button::Listener,
    public VSTNode::VSTNodeListener,
    private ControllableContainerListener
{
public:
    VSTNodeContentUI();
    ~VSTNodeContentUI();
    OwnedArray<FloatSliderUI> paramSliders;

    VSTNode* vstNode;

    TextButton VSTListShowButton;
    TextButton showPluginWindowButton;

    ScopedPointer<ParameterUI> midiDeviceChooser;

    ScopedPointer<ParameterUI> activityBlink;

    void init() override;
    void resized()override;

    void updateVSTParameters();
    void newVSTSelected() override;
    

    void layoutSliderParameters (Rectangle<int> pArea);
    
    void buttonClicked (Button* button) override;

private:
    void createPluginWindow();
    void closePluginWindow();

    void controllableAdded (ControllableContainer*, Controllable* c)override;
    void controllableRemoved (ControllableContainer*, Controllable* c)override;
    void controllableContainerAdded (ControllableContainer*, ControllableContainer* cc)override;
    void controllableContainerRemoved (ControllableContainer*, ControllableContainer* cc) override;
    void controllableFeedbackUpdate (ControllableContainer* originContainer, Controllable* c) override;


    static void vstSelected (int modalResult, Component*   originComp);
    void handleCommandMessage (int )override;

    bool isDirty;

};


#endif  // VSTNODEUI_H_INCLUDED
