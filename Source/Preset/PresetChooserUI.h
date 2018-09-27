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

#ifndef PRESETCHOOSER_H_INCLUDED
#define PRESETCHOOSER_H_INCLUDED

#include "../Controllable/Parameter/ParameterContainer.h"
#include "../JuceHeaderUI.h"

class PresetChooserUI : public ComboBox,
    public ComboBox::Listener,
    private ControllableContainer::Listener
{
public:
    PresetChooserUI (ParameterContainer* _container);
    ~PresetChooserUI();

    WeakReference<ParameterContainer> container;


    virtual void updatePresetComboBox (bool forceUpdate = false);
    virtual void comboBoxChanged (ComboBox* comboBoxThatHasChanged) override;

    virtual void controllableContainerPresetLoaded (ControllableContainer*) override;


    static void fillWithPresets (ComboBox* cb, const String& filter, bool showSaveCurrent = true) ;

};



#endif  // PRESETCHOOSER_H_INCLUDED
