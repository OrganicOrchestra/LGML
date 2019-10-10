

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


#include "../UI/ConnectableNodeContentUI.h"
#include "PdNode.h"


class PdNodeContentUI:
    public ConnectableNodeContentUI,
    private ControllableContainerListener
{
public:
    PdNodeContentUI();
    ~PdNodeContentUI();
    OwnedArray<ParameterUI> paramSliders;


    PdNode* pdNode;

//    TextButton VSTListShowButton;
    

    std::unique_ptr<ParameterUI> midiDeviceChooser;
    std::unique_ptr<ParameterUI> fileChooser;

    std::unique_ptr<ParameterUI> activityBlink;


    void init() override;
    void resized()override;

    void updatePdParameters();

    

    void layoutSliderParameters (Rectangle<int> pArea);
    
//    void buttonClicked (Button* button) override;

private:
//    void createPluginWindow();
//    void closePluginWindow();

    void childControllableAdded (ControllableContainer*, Controllable* c)override;
    void childControllableRemoved (ControllableContainer*, Controllable* c)override;
    void controllableContainerAdded (ControllableContainer*, ControllableContainer* cc)override;
    void controllableContainerRemoved (ControllableContainer*, ControllableContainer* cc) override;
    
    void newPdPatchLoaded();

    static void pdSelected (int modalResult, Component*   originComp);
    void handleCommandMessage (int )override;

    bool isDirty;


    

};

#if NON_INCREMENTAL_COMPILATION
    #include "PdNodeUI.cpp"
#endif

