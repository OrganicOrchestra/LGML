

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



#include "../../Controllable/Parameter/UI/SliderUI.h"
#include "../../MIDI/MIDIUIHelper.h"

#include "../UI/ConnectableNodeContentUI.h"
#include "PdNode.h"


class PdNodeContentUI:
    public ConnectableNodeContentUI,
    private ControllableContainerListener,
    private ParameterBase::AsyncListener
{
public:
    PdNodeContentUI();
    ~PdNodeContentUI();
    OwnedArray<ParameterUI> paramSliders;


    PdNode* pdNode;

//    TextButton VSTListShowButton;
//    TextButton showPluginWindowButton;

    ScopedPointer<ParameterUI> midiDeviceChooser;

    ScopedPointer<ParameterUI> activityBlink;

    void init() override;
    void resized()override;

    void updatePdParameters();

    

    void layoutSliderParameters (Rectangle<int> pArea);
    
//    void buttonClicked (Button* button) override;

private:
//    void createPluginWindow();
//    void closePluginWindow();

    void controllableAdded (ControllableContainer*, Controllable* c)override;
    void controllableRemoved (ControllableContainer*, Controllable* c)override;
    void controllableContainerAdded (ControllableContainer*, ControllableContainer* cc)override;
    void controllableContainerRemoved (ControllableContainer*, ControllableContainer* cc) override;
    
    void newPdPatchLoaded();

    static void pdSelected (int modalResult, Component*   originComp);
    void handleCommandMessage (int )override;
    void newMessage (const ParameterBase::ParamWithValue&) override;
    bool isDirty;

};


