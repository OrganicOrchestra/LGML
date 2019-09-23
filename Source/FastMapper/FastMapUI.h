/*
 ==============================================================================

 Copyright © Organic Orchestra, 2017

 This file is part of LGML. LGML is a software to manipulate sound in real-time

 This program is free software; you can redistribute it and/or modify
 it under the terms of the GNU General Public License as published by
 the Free Software Foundation (version 3 of the License).

 This program is distributed in the hope that it will be useful,
 but WITHOUT ANY WARRANTY; without even the implied warranty of
 MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.

 ==============================================================================
 */

#ifndef FASTMAPUI_H_INCLUDED
#define FASTMAPUI_H_INCLUDED

#include "FastMap.h"

#include "../UI/Inspector/InspectableComponent.h"
#include "../Controllable/Parameter/UI/ParameterProxyUI.h"
#include "../Controllable/Parameter/UI/RangeParameterUI.h"

class FastMapUI :
    public InspectableComponent,
    private  ControllableReferenceUI::Listener,
    private Button::Listener,
    private ParameterProxy::ParameterProxyListener

{
public:
    FastMapUI (FastMap* f);
    virtual ~FastMapUI();




    std::unique_ptr<ParameterUI> enabledUI,fullSyncUI;

    ParameterProxyUI refUI;
    RangeParameterUI inRangeUI;


    ParameterProxyUI targetUI;
    RangeParameterUI outRangeUI;
    std::unique_ptr<ParameterUI> invertUI;
    std::unique_ptr<ParameterUI> toggleUI;



    FastMap* fastMap;
    ImageButton removeBT;


    void paint (Graphics& g) override;
    void resized() override;

    //  virtual void choosedControllableChanged(ControllableReferenceUI*,Controllable *) override;
    //  virtual void fastMapTargetChanged(FastMap *) override;


    
    void buttonClicked (Button* b) override;

    void linkedParamChanged (ParameterProxy*) override;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (FastMapUI)

};




#endif  // FASTMAPUI_H_INCLUDED
