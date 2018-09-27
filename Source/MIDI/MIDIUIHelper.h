/*
 ==============================================================================

 Copyright © Organic Orchestra, 2017

 This file is part of LGML. LGML is a software to manipulate sound in realtime

 This program is free software; you can redistribute it and/or modify
 it under the terms of the GNU General Public License as published by
 the Free Software Foundation (version 3 of the License).

 This program is distributed in the hope that it will be useful,
 but WITHOUT ANY WARRANTY; without even the implied warranty of
 MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.

 ==============================================================================
 */

#ifndef MIDIUIHELPER_H_INCLUDED
#define MIDIUIHELPER_H_INCLUDED
#pragma once

#include "MIDIManager.h"
#include "../JuceHeaderUI.h"

class MIDIDeviceChooser : public ComboBox, public MIDIManager::MIDIManagerListener
{
public:
    explicit MIDIDeviceChooser (bool isInputChooser);
    virtual ~MIDIDeviceChooser();

    bool isInputChooser;

    String currentDeviceName;

    void fillDeviceList();
    void setSelectedDevice (const String& deviceName, bool silent = true);


    virtual void midiInputsChanged() override;
    virtual void midiOutputsChanged() override;
};

#endif  // MIDIUIHELPER_H_INCLUDED
