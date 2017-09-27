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

#include "PresetChooserUI.h"
#include "../Utils/DebugHelpers.h"
#include "../Controllable/Parameter/ParameterContainer.h"


enum PresetChoice
{
    SaveCurrent = -3,
    SaveToNew = -2,
    ResetToDefault = -1,
    deleteStartId = 1000
};


PresetChooserUI::PresetChooserUI (ParameterContainer* _container) :
    container (_container),
    ComboBox ("Preset")
{
    updatePresetComboBox();
    ComboBox::addListener (this);
    container->addControllableContainerListener (this);
    setTextWhenNothingSelected ("Preset");
    setTooltip ("Set the current preset at :\n" + container->currentPresetName->getControlAddress() + " <presetName>");
}

PresetChooserUI::~PresetChooserUI()
{
    ComboBox::removeListener (this);

    if (auto c = container.get())c->removeControllableContainerListener (this);
}

void PresetChooserUI::fillWithPresets (ComboBox* cb, const  String& filter, bool _showSaveCurrent)
{
    cb->clear();

    if (_showSaveCurrent) cb->addItem ("Save current preset", SaveCurrent);

    cb->addItem ("Save to new preset", SaveToNew);
    cb->addItem ("Reset to default", ResetToDefault);

    int pIndex = 1;
    PresetManager* pm = PresetManager::getInstance();

    for (auto& pre : pm->presets)
    {

        if (pre->filter == filter)
        {
            pre->presetId = pIndex;
            cb->addItem (pre->name, pre->presetId);
            pIndex++;
        }
    }

    for (auto& pre : pm->presets)
    {

        if (pre->filter == filter)
        {
            cb->addItem ("delete " + pre->name, PresetChoice::deleteStartId + pre->presetId);
        }
    }

}

void PresetChooserUI::updatePresetComboBox (bool forceUpdate)
{

    bool emptyFilter = container->getPresetFilter().isEmpty();
    setEnabled (!emptyFilter);

    if (!emptyFilter)
    {
        fillWithPresets (this, container->getPresetFilter(), container->currentPreset != nullptr);

        if (container->currentPreset != nullptr) this->setSelectedId (container->currentPreset->presetId, forceUpdate ? sendNotification : dontSendNotification);
    }
}

void PresetChooserUI::comboBoxChanged (ComboBox* cb)
{

    int presetID = cb->getSelectedId();

    if (container->currentPreset != nullptr && presetID == container->currentPreset->presetId) return;

    if (presetID == PresetChoice::SaveCurrent)
    {
        bool result = container->saveCurrentPreset();

        if (result) cb->setSelectedId (container->currentPreset->presetId, NotificationType::dontSendNotification);
        else cb->setSelectedItemIndex (-1, NotificationType::dontSendNotification);

    }
    else if (presetID == PresetChoice::SaveToNew)
    {
        AlertWindow nameWindow ("Save a new Preset", "Choose a name for the new preset", AlertWindow::AlertIconType::QuestionIcon, this);
        nameWindow.addTextEditor ("newPresetName", "New Preset");
        nameWindow.addButton ("OK", 1, KeyPress (KeyPress::returnKey));
        nameWindow.addButton ("Cancel", 0, KeyPress (KeyPress::escapeKey));

        int nameResult = nameWindow.runModalLoop();

        if (nameResult)
        {
            String presetName = nameWindow.getTextEditorContents ("newPresetName");
            PresetManager::Preset* p = container->saveNewPreset (presetName);
            cb->clear (NotificationType::dontSendNotification);
            updatePresetComboBox();
            cb->setSelectedId (p->presetId, dontSendNotification);
        }
        else
        {
            cb->setSelectedItemIndex (-1, dontSendNotification);
        }


    }
    else if (presetID == PresetChoice::ResetToDefault)   //Reset to default
    {
        container->resetFromPreset();
        updatePresetComboBox (true);
        cb->setSelectedItemIndex (-1, NotificationType::dontSendNotification);
    }
    else if (presetID >= 0 && presetID < PresetChoice::deleteStartId)
    {
        String nameOfPreset = cb->getItemText (cb->getSelectedItemIndex());

        container->currentPresetName->setValue (nameOfPreset);

    }
    else if (presetID >= PresetChoice::deleteStartId)
    {
        bool ok = AlertWindow::showOkCancelBox (AlertWindow::AlertIconType::QuestionIcon, "Oh man, d'ya know watcha doin' ?", "Do you REALLY want to delete this preset ?\nLike, really really ?\nJust think about it man.", "Oh yeah", "F* No");

        if (ok)
        {
            PresetManager* pm = PresetManager::getInstance();
            int originId = cb->getSelectedId() - PresetChoice::deleteStartId - 1;
            String originText = cb->getItemText (cb->getNumItems() - container->getNumPresets() * 2 + originId);
            PresetManager::Preset* pre = pm->getPreset (container->getPresetFilter(), originText);
            pm->presets.removeObject (pre);

            container->currentPreset = nullptr;
            updatePresetComboBox (true);
        }
        else
        {
            //reselect last Id
            if (container->currentPreset != nullptr)
            {
                cb->setSelectedId (container->currentPreset->presetId, juce::dontSendNotification);
            }
            else
            {
                cb->setSelectedId (0, juce::dontSendNotification);
            }

        }

    }
    else
    {
        jassertfalse;
    }

}

void PresetChooserUI::controllableContainerPresetLoaded (ControllableContainer*)
{
    updatePresetComboBox (false);
}
