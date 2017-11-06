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

#include "PresetManager.h"
#include "../Controllable/Parameter/ParameterContainer.h"

juce_ImplementSingleton (PresetManager)


PresetManager::PresetManager()
{
}

PresetManager::~PresetManager()
{
    presets.clear();
}

PresetManager::Preset* PresetManager::addPresetFromControllableContainer (const String& name, const String& filter, ParameterContainer* container, bool recursive, bool includeNotExposed)
{
    //Array<PresetValue *> vPresets;
    Preset* pre = getPreset (filter, name);
    bool presetExists = pre != nullptr;

    if (!presetExists)
    {
        pre = new Preset (name, filter);
    }
    else
    {
        pre->clear();
    }

    for (auto& p : container->getAllParameters (recursive, includeNotExposed))
    {
        if (!p->isPresettable || !p->isEditable) continue;

        if (p == (Parameter*)container->currentPresetName) { continue; }

        if (!p->isControllableExposed && !includeNotExposed) continue;

        pre->addPresetValue (p->getControlAddress (container), var (p->value));
    }

    if (!recursive)
    {
        for (auto& cc : container->getContainersOfType<ParameterContainer> (false))
        {
            if (cc->currentPresetName->stringValue().isNotEmpty())
            {
                pre->addPresetValue (cc->currentPresetName->getControlAddress (container), cc->currentPresetName->value);
            }
        }
    }


    if (!presetExists) presets.add (pre);

    return pre;
}

PresetManager::Preset* PresetManager::getPreset (String filter, const String& name) const
{
    for (auto& pre : presets)
    {
        if (pre->filter == filter && pre->name == name) return pre;
    }

    return nullptr;
}



void PresetManager::removePresetForIdx (int idx)
{
    if (idx > 0 && idx < presets.size())
    {
        presets.remove (idx);
    }

}

int PresetManager::getNumPresetForFilter (const String& filter) const
{
    int num = 0;

    for (auto& pre : presets)
    {

        if (pre->filter == filter)
        {
            num++;
        }
    }

    return num;
}

void PresetManager::deleteAllUnusedPresets (ParameterContainer* rootContainer)
{
    Array<Preset*> presetsToRemove;
    auto allContainers = rootContainer->getContainersOfType<ParameterContainer> (true);

    for (auto& p : presets)
    {
        bool isUsed = false;

        for (auto& cc : allContainers)
        {
            if (cc->getPresetFilter() == p->filter)
            {
                isUsed = true;
                break;
            }
        }

        if (!isUsed) presetsToRemove.add (p);
    }

    int numPresetsToRemove = presetsToRemove.size();

    for (auto& p : presetsToRemove) presets.removeObject (p);

    LOG ("Cleaned up " << numPresetsToRemove << " unused presets, now " << presets.size() << "presets");
}

int PresetManager::deletePresetsForContainer (ParameterContainer* container, bool recursive)
{
    if (container == nullptr) return 0;

    const String filter = container->getPresetFilter();

    Array<Preset*> presetsToRemove;

    for (auto& p : presets)
    {
        if (p->filter == filter) presetsToRemove.add (p);
    }

    int numPresetsDeleted = presetsToRemove.size();

    for (auto& p : presetsToRemove) presets.removeObject (p);

    if (recursive)
    {
        for (auto& cc : container->getContainersOfType<ParameterContainer> (false))
        {
            if (cc)
            {
                numPresetsDeleted += deletePresetsForContainer (cc, true);
            }
        }
    }

    return numPresetsDeleted;
}

void PresetManager::clear()
{
    presets.clear();
}

DynamicObject* PresetManager::getObject()
{
    auto data = new DynamicObject();
    var presetDatas;

    for (auto& p : presets)
    {
        presetDatas.append (p->getObject());
    }

    data->setProperty ("presets", presetDatas);

    return data;
}

void PresetManager::configureFromObject (DynamicObject* data)
{
    clear();

    Array<var>* presetDatas = data->getProperty ("presets").getArray();

    if (presetDatas == nullptr)
    {
        //DBG("no preset Loaded");
        return;
    }

    for (auto& presetData : *presetDatas)
    {
        Preset* pre = new Preset (presetData.getDynamicObject()->getProperty ("name"), presetData.getDynamicObject()->getProperty ("filter"));
        pre->configureFromObject (presetData.getDynamicObject());

        presets.add (pre);
    }

}



//////////////////////////////
// Presets


void PresetManager::Preset::addPresetValue (const String& controlAddress, var value)
{
    presetValues.add (new PresetValue (controlAddress, value));
}

void PresetManager::Preset::addPresetValues (Array<PresetValue*> _presetValues)
{
    presetValues.addArray (_presetValues);
}

var PresetManager::Preset::getPresetValue (const String& targetControlAddress)
{
    for (auto& pv : presetValues)
    {

        if (pv->paramControlAddress == targetControlAddress) return pv->presetValue;
    }

    return var();
}

void PresetManager::Preset::clear()
{
    presetValues.clear();
}


DynamicObject* PresetManager::Preset::getObject()
{
    auto data = new DynamicObject();
    data->setProperty ("name", name);
    data->setProperty ("filter", filter);
    var presetValuesData;

    for (auto& pv : presetValues)
    {
        var pvData (new DynamicObject());
        pvData.getDynamicObject()->setProperty (ControllableContainer::controlAddressIdentifier, pv->paramControlAddress);
        pvData.getDynamicObject()->setProperty ("value", pv->presetValue);
        presetValuesData.append (pvData);
    }

    data->setProperty ("values", presetValuesData);
    return data;
}

void PresetManager::Preset::configureFromObject (DynamicObject* data)
{

    Array<var>* pvDatas = data->getProperty ("values").getArray();

    if (pvDatas != nullptr)
    {
        for (auto& pvData : *pvDatas)
        {
            addPresetValue (pvData.getProperty (ControllableContainer::controlAddressIdentifier, var()), pvData.getProperty ("value", var()));
        }
    }
}
