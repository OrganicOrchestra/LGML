/*
  ==============================================================================

    PresetManager.cpp
    Created: 25 Apr 2016 2:02:05pm
    Author:  bkupe

  ==============================================================================
*/

#include "PresetManager.h"
#include "ControllableContainer.h"

juce_ImplementSingleton(PresetManager)


PresetManager::PresetManager()
{
}

PresetManager::~PresetManager()
{
    presets.clear();
}

PresetManager::Preset * PresetManager::addPresetFromControllableContainer(const String &name, String filter, ControllableContainer * container, bool recursive, bool includeNotExposed)
{
    //Array<PresetValue *> vPresets;
    Preset * pre = new Preset(name,filter);
    for (auto &p : container->getAllParameters(recursive,includeNotExposed))
    {
        if (!p->isPresettable) continue;
        if (!p->isControllableExposed && !includeNotExposed) continue;

        DBG("Add preset value " << p->niceName << " > " <<  p->stringValue());

        //PresetValue * preVal = new PresetValue(p->controlAddress,p->value.clone());
        //vPresets.add(preVal);
        pre->addPresetValue(p->getControlAddress(container), p->value.clone());

    }


    presets.add(pre);

    return pre;
}

ComboBox * PresetManager::getPresetSelector(String filter)
{
    ComboBox * cb = new ComboBox("Presets");
    fillWithPresets(cb, filter );

    return cb;
}

PresetManager::Preset * PresetManager::getPreset(String filter, const String & name)
{
    for (auto &pre : presets)
    {
        if (pre->filter == filter && pre->name == name) return pre;
    }

    return nullptr;
}

void PresetManager::fillWithPresets(ComboBox * cb, String filter)
{
    cb->clear();
    cb->addItem("Save current preset", SaveCurrent);
    cb->addItem("Save to new preset", SaveToNew);
    cb->addItem("Reset to default", ResetToDefault);

    int pIndex = 1;
    for (auto &pre : presets)
    {
        if (pre->filter == filter)
        {
            pre->presetId = pIndex;
            cb->addItem(pre->name, pre->presetId);
            pIndex++;
        }
    }
}

void PresetManager::clear()
{
    presets.clear();
}
