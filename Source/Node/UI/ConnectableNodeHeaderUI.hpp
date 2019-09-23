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


#ifndef CONNECTABLENODEHEADERUI_H_INCLUDED
#define CONNECTABLENODEHEADERUI_H_INCLUDED
#pragma once

#include "../../Controllable/Parameter/UI/StringParameterUI.h"
#include "../../Controllable/Parameter/UI/BoolToggleUI.h"
#include "../ConnectableNode.h"
class PresetChooserUI;


class VuMeter;
class ConnectableNodeUI;

class ConnectableNodeHeaderUI : public juce::Component,
    public Button::Listener,
    private ControllableContainerListener,
    public ConnectableNode::ConnectableNodeListener,
    public TooltipClient
{
public:


    ConnectableNodeHeaderUI();
    virtual ~ConnectableNodeHeaderUI();

    ConnectableNode* node;
    ConnectableNodeUI* nodeUI;

    std::unique_ptr<StringParameterUI> titleUI;
    std::unique_ptr<StringParameterUI> descriptionUI;
    std::unique_ptr<ParameterUI> enabledUI;
    std::unique_ptr<VuMeter> vuMeterIn;
    std::unique_ptr<VuMeter> vuMeterOut;


    
    TextButton miniModeBT;
    std::unique_ptr<PresetChooserUI> presetChooser;


    virtual void setNodeAndNodeUI (ConnectableNode* node, ConnectableNodeUI* nodeUI);
    virtual void init();


    virtual void resized() override;

    void updateVuMeters();

    bool bMiniMode;
    virtual void setMiniMode (bool value);

    // Inherited via Listeners
    virtual void nodeParameterChangedAsync (ConnectableNode*, ParameterBase*) override;

    virtual void buttonClicked (Button*) override;
    


    void numAudioInputChanged (ConnectableNode*, int /*newNumInput*/) override;
    void numAudioOutputChanged (ConnectableNode*, int /*newNumOutput*/) override;


    void handleCommandMessage (int id) override;
    enum
    {
        repaintId,
        audioInputChangedId,
        audioOutputChangedId
    } DrawingCommand;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (ConnectableNodeHeaderUI)

    String getTooltip() override;


};

#include "ConnectableNodeHeaderUI.ipp"


#endif  // CONNECTABLENODEHEADERUI_H_INCLUDED
