/*
  ==============================================================================

    PresetManager.cpp
    Created: 25 Apr 2016 2:02:05pm
    Author:  bkupe

  ==============================================================================
*/

#include "PresetManager.h"
#include "ControllableContainer.h"
#include "NodeFactory.h"

juce_ImplementSingleton(PresetManager)


PresetManager::PresetManager()
{
}

PresetManager::~PresetManager()
{
	nodePresets.clear();
}

PresetManager::NodePreset * PresetManager::addNodePreset(const String &name, NodeType nodeType, ControllableContainer * container, bool recursive, bool includeNotExposed)
{
	//Array<PresetValue *> vPresets;
	NodePreset * pre = new NodePreset(nodeType, name);
	for (auto &p : container->getAllParameters(recursive,includeNotExposed))
	{
		if (!p->isPresettable) continue;
		if (!p->isControllableExposed && !includeNotExposed) continue;

		DBG("Add preset value " << p->niceName << " > " <<  p->stringValue());

		//PresetValue * preVal = new PresetValue(p->controlAddress,p->value.clone());
		//vPresets.add(preVal);
		pre->addPresetValue(p->getControlAddress(container), p->value.clone());

	}

	
	nodePresets.add(pre);

	return pre;
}

ComboBox * PresetManager::getNodePresetSelector(NodeType nodeType)
{
	ComboBox * cb = new ComboBox("Presets");
	fillWithNodePresets(cb, nodeType);
	
	return cb;
}

PresetManager::NodePreset * PresetManager::getNodePreset(NodeType nodeType, const String & name)
{
	for (auto &pre : nodePresets)
	{
		if (pre->nodeType == nodeType && pre->name == name) return pre;
	}

	return nullptr;
}

void PresetManager::fillWithNodePresets(ComboBox * cb, NodeType nodeType)
{
	cb->addItem("Save current preset", SaveCurrent);
	cb->addItem("Save to new preset", SaveToNew);
	cb->addItem("Reset to default", ResetToDefault);

	int pIndex = 1;
	for (auto &pre : nodePresets)
	{
		if (pre->nodeType == nodeType)
		{
			pre->presetId = pIndex;
			cb->addItem(pre->name, pre->presetId);
			pIndex++;
		}
	}
}

void PresetManager::clear()
{
	nodePresets.clear();
}
