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

#if !ENGINE_HEADLESS

#include "PresetChooserUI.h"
#include "../Utils/DebugHelpers.h"
#include "../Controllable/Parameter/ParameterContainer.h"
#include "PresetManager.h"
#include "Preset.h"
#include "../UI/Style.h"

enum PresetChoice
{
    SaveCurrent = -3,
    SaveToNew = -2,
    ResetToCurrent = -1,
    deleteStartId = 1000,
    otherStartId = 2000
};


PresetChooserUI::PresetChooserUI (ParameterContainer* _container) :
    container (_container),
    ComboBox ("Preset")
{
    PresetManager::getInstance()->presetListeners.add(this);
    updatePresetComboBox();
    ComboBox::addListener (this);
    container->addControllableContainerListener (this);
    setTextWhenNothingSelected ("Preset");
    setTooltip (juce::translate("Set the current preset at")+" :\n" + getPresetable()->currentPresetName->getControlAddress().toString() + " <presetName>");
    getPresetable()->presetableListeners.add(this);
    setOpaque(true);
    setPaintingIsUnclipped(true);
    LGMLUIUtils::optionallySetBufferedToImage(this);
    setRepaintsOnMouseActivity(false);
    
}

PresetChooserUI::~PresetChooserUI()
{
    PresetManager::getInstance()->presetListeners.remove(this);
    if(auto * pre = getPresetable())
        pre->presetableListeners.remove(this);
    ComboBox::removeListener (this);

    if (auto c = container.get())c->removeControllableContainerListener (this);
}

void PresetChooserUI::rebuildAvailable(){
    if(!container){
        return;
    }
    int pIndex = 1;
    String filter = container->presetable->getPresetFilter();
    PresetManager* pm = PresetManager::getInstance();
    availablePresets = pm->getPresetsForFilter(filter);
    otherPresets = pm->getPresetsForType(container->presetable->getType(),container);
    auto * p = getPresetable()->currentPreset;
    selectedPresetId = 0;
    for (auto& pre : availablePresets)
    {
        if(pre && pre==p){
            selectedPresetId = pIndex;
        }
        pIndex++;

    }
    if(selectedPresetId==0){
        pIndex = PresetChoice::otherStartId+1;
        for (auto& pre : otherPresets)
        {
            if(pre && pre==p){
                selectedPresetId = pIndex;
            }
            pIndex++;
            
        }
    }

}
void PresetChooserUI::fillWithPresets (ComboBox* cb)
{

    cb->clear();
    if(!container) return;

    cb->addItem (juce::translate("Save to new preset"), SaveToNew);
    if (getPresetable()->hasPresetLoaded()){
        cb->addItem (juce::translate("Save current preset"), SaveCurrent);
        cb->addItem (juce::translate("Reload current preset"), ResetToCurrent);
    }


    int pIndex = 1;

    rebuildAvailable();

    for (auto& pre : availablePresets)
    {
            cb->addItem (pre->getPresetName(), pIndex);
            pIndex++;
    }
    pIndex = 1;
    for (auto& pre : availablePresets)
    {
            cb->addItem (juce::translate("delete")+" " + pre->getPresetName(), PresetChoice::deleteStartId +pIndex);
            pIndex++;
    }

    auto menu = cb->getRootMenu();
    PopupMenu sameTypePresets;
    pIndex = 1;
    for(auto & p : otherPresets){
        sameTypePresets.addItem(otherStartId+pIndex, p->getPresetName() + String("(123)").replace("123",p->getOriginContainer()->getControlAddress().toString()));
        pIndex++;
    }

    if(sameTypePresets.getNumItems())
        menu->addSubMenu(juce::translate("Other Presets In Patch"),sameTypePresets,true);
}

void PresetChooserUI::updatePresetComboBox (bool forceUpdate)
{
    if(!container){
        LOGW("old presetChooserUI trying to refresh");
//        jassertfalse;
        return ;
    }
    bool emptyFilter = getPresetable()->getPresetFilter().isEmpty();
    setEnabled (!emptyFilter);

    if (!emptyFilter)
    {
        fillWithPresets (this);
        selectedPresetId = findSelectedId();
        setSelectedId (selectedPresetId, forceUpdate ? sendNotification : dontSendNotification);
    }
}

void PresetChooserUI::comboBoxChanged (ComboBox* cb)
{
    if(!container) return;

    int presetIDCmd = cb->getSelectedId();

    if (presetIDCmd == selectedPresetId) return;



    if (presetIDCmd == PresetChoice::SaveCurrent)
    {
        getPresetable()->saveCurrentPreset(this);
//        rebuildAvailable(container->getPresetFilter());
        cb->setSelectedId (selectedPresetId, NotificationType::dontSendNotification);


    }
    else if (presetIDCmd == PresetChoice::SaveToNew)
    {
        AlertWindow nameWindow ("Save a new Preset", "Choose a name for the new preset", AlertWindow::AlertIconType::QuestionIcon, this);
        nameWindow.addTextEditor ("newPresetName", "New Preset");
        nameWindow.addButton ("OK", 1, KeyPress (KeyPress::returnKey));
        nameWindow.addButton ("Cancel", 0, KeyPress (KeyPress::escapeKey));

        int nameResult = nameWindow.runModalLoop();

        if (nameResult)
        {
            String presetName = nameWindow.getTextEditorContents ("newPresetName");
            getPresetable()->saveNewPreset (presetName,this);

        }

        else{
        cb->setSelectedId (selectedPresetId, dontSendNotification);
        }


    }
    else if (presetIDCmd == PresetChoice::ResetToCurrent)   //Reset to default
    {
        getPresetable()->resetToCurrentPreset();
        updatePresetComboBox (true);
        

    }
    else if (presetIDCmd >= 0 && presetIDCmd < PresetChoice::deleteStartId)
    {
//        selectedPresetId = presetIDCmd;
        String nameOfPreset = cb->getItemText (cb->getSelectedItemIndex());

        getPresetable()->currentPresetName->setValue (nameOfPreset);


    }
    else if (presetIDCmd >= PresetChoice::deleteStartId && presetIDCmd < PresetChoice::otherStartId)
    {
        bool ok = AlertWindow::showOkCancelBox (AlertWindow::AlertIconType::QuestionIcon, juce::translate("Oh man, d'ya know watcha doin' ?"), juce::translate("Do you REALLY want to delete this preset ?\nLike, really really ?\nJust think about it man."), juce::translate("Oh yeah"), juce::translate("F* No"));

        if (ok)
        {
            PresetManager* pm = PresetManager::getInstance();
            int index = cb->indexOfItemId(getSelectedId()-PresetChoice::deleteStartId);
            String originText = cb->getItemText (index);
            Preset* pre = pm->getPreset (getPresetable()->getPresetFilter(), originText);
            if(pre){
                getPresetable()->deletePreset(pre);
            }
            
        }
        else
        {
            //reselect last Id
            if (getPresetable()->hasPresetLoaded())
            {
                cb->setSelectedId (selectedPresetId, juce::dontSendNotification);
            }
            else
            {
                cb->setSelectedId (0, juce::dontSendNotification);
            }

        }

    }
    else if (presetIDCmd > PresetChoice::otherStartId){
        int originId = cb->getSelectedId() - PresetChoice::otherStartId - 1;
        int i = 0;
        Preset * pre=nullptr;
        for(auto & p : otherPresets){if(i==originId){pre = p;break;}i++;}
        if(pre){
            getPresetable()->currentPresetName->setValue (pre->getPresetName() + "(" + pre->getOriginUID()+")");

        }else{
            jassertfalse;
        }
    }
    else
    {
        jassertfalse;
    }

}

void PresetChooserUI::controllableContainerPresetLoaded (ControllableContainer*,Preset * )
{
    auto wkC = WeakReference<Component>(this);
    MessageManager::callAsync([wkC](){
        if(wkC){
            auto pui = static_cast<PresetChooserUI*> (wkC.get());
            pui->updatePresetComboBox (false);
        }
    });

}
void PresetChooserUI::presetRemoved(Preset *) {

    auto wkC = WeakReference<Component>(this);
    MessageManager::callAsync([wkC](){
        if(wkC){
            auto pui = static_cast<PresetChooserUI*> (wkC.get());
            pui->updatePresetComboBox (false);
        }
    });


};
void PresetChooserUI::presetAdded(Preset *) {
    updatePresetComboBox (false);

};


int PresetChooserUI::findSelectedId(){
    int i = 0;
    for(auto & p:availablePresets){
        i++;
        if(p==container->presetable->currentPreset){
            return i;
        }
    }
    i = PresetChoice::otherStartId;
    for(auto & p:otherPresets){
        i++;
        if(p==container->presetable->currentPreset){
            return i;
        }
    }
    return 0;

}

#endif
