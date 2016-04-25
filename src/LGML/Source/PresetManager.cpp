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

void PresetManager::addPreset(const String &name, Preset::Type presetType, ControllableContainer * container, bool recursive, bool includeNotExposed)
{
	
	Array<PresetValue *> nPresets;
	for (auto &p : container->getAllParameters())
	{
		if (!p->isPresettable) continue;
		if (!p->isControllableExposed && !includeNotExposed) continue;

		PresetValue * pre = new PresetValue(p->controlAddress,p->value.clone());
		nPresets.add(pre);
	}

	if (recursive)
	{
		//recursive not functional for now
	}

	Preset * pre = new Preset(presetType, name);
	pre->addPresetValues(nPresets);
}

ComboBox * PresetManager::getPresetSelector(Preset::Type type)
{
	ComboBox * cb = new ComboBox("Presets");
	int pIndex = 0;
	for (auto &pre : presets)
	{
		if (pre->type == type)
		{
			cb->addItem(pre->name, pIndex);
			pIndex++;
		}
	}
	
	return cb;
}

PresetManager::Preset * PresetManager::getPreset(Preset::Type presetType, const String & name)
{
	for (auto &pre : presets)
	{
		if (pre->type == presetType && pre->name == name) return pre;
	}

	return nullptr;
}
