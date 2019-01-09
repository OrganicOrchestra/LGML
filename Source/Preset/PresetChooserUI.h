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
#include "PresetManager.h"
#include "../JuceHeaderUI.h"

class PresetChooserUI : public ComboBox,
    public ComboBox::Listener,
    private ControllableContainer::Listener,
    private PresetManager::Listener,
    private Presetable::Listener
{
public:
    PresetChooserUI (ParameterContainer* _container);
    ~PresetChooserUI();

    WeakReference<ParameterContainer> container;
    Array<WeakReference<Preset>> availablePresets,otherPresets;

    int selectedPresetId;


    void updatePresetComboBox (bool forceUpdate = false);
    void comboBoxChanged (ComboBox* comboBoxThatHasChanged) final;

    virtual void controllableContainerPresetLoaded (ControllableContainer*,Preset *) final;


    void fillWithPresets (ComboBox* cb) ;

private:
    int findSelectedId();
    Presetable * getPresetable(){return container?container->presetable.get():nullptr;}
    void presetRemoved(Preset *p) final;
    void presetAdded(Preset *p) final;
    
    void rebuildAvailable();

};



#endif  // PRESETCHOOSER_H_INCLUDED
