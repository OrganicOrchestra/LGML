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

PresetManager::Preset * PresetManager::addPresetFromControllableContainer(const String &name, const String & filter, ControllableContainer * container, bool recursive, bool includeNotExposed)
{
    //Array<PresetValue *> vPresets;
    Preset * pre = new Preset(name,filter);
    for (auto &p : container->getAllParameters(recursive,includeNotExposed))
    {
        if (!p->isPresettable) continue;
        if(p==(Parameter*)container->currentPresetName){ continue; }
        if (!p->isControllableExposed && !includeNotExposed) continue;

        //DBG("Add preset value " << p->niceName << " > " <<  p->value.toString() << p->stringValue());

        //PresetValue * preVal = new PresetValue(p->controlAddress,p->value.clone());
        //vPresets.add(preVal);
        pre->addPresetValue(p->getControlAddress(container), p->value.clone());

    }

    presets.add(pre);



    return pre;
}

PresetManager::Preset * PresetManager::getPreset(String filter, const String & name) const
{
    for (auto &pre : presets)
    {
        if (pre->filter == filter && pre->name == name) return pre;
    }

    return nullptr;
}

void PresetManager::fillWithPresets(ComboBox * cb,const  String & filter, bool showSaveCurrent) const
{
    cb->clear();
    if(showSaveCurrent) cb->addItem("Save current preset", SaveCurrent);
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
    for (auto &pre : presets)
    {

        if (pre->filter == filter)
        {
            cb->addItem("delete "+pre->name, PresetChoice::deleteStartId + pre->presetId);
        }
    }

}

void PresetManager::removePresetForIdx(int idx){
  if(idx >0 && idx < presets.size()){
    presets.remove(idx);
  }

}

int PresetManager::getNumPresetForFilter (const String & filter) const{
  int num = 0;
  for (auto &pre : presets)
  {

    if (pre->filter == filter)
    {
      num++;
    }
  }
  return num;
}
int PresetManager::getNumOption(){
  return 3;
}
void PresetManager::clear()
{
    presets.clear();
}

var PresetManager::getJSONData()
{
	var data(new DynamicObject());
	var presetDatas;

	for (auto &p : presets)
	{
		presetDatas.append(p->getJSONData());
	}

	data.getDynamicObject()->setProperty("presets", presetDatas);

	return data;
}

void PresetManager::loadJSONData(var data)
{
	clear();

	Array<var> * presetDatas = data.getDynamicObject()->getProperty("presets").getArray();
    if(presetDatas==nullptr){
        //DBG("no preset Loaded");
        return;
    }
	for (auto &presetData : *presetDatas)
	{
		Preset * pre = new Preset(presetData.getDynamicObject()->getProperty("name"), presetData.getDynamicObject()->getProperty("filter"));
		pre->loadJSONData(presetData);

		presets.add(pre);
	}

}



//////////////////////////////
// Presets


void PresetManager::Preset::addPresetValue(const String &controlAddress, var value)
{
  presetValues.add(new PresetValue(controlAddress, value));
}

void PresetManager::Preset::addPresetValues(Array<PresetValue *> _presetValues)
{
  presetValues.addArray(_presetValues);
}

var PresetManager::Preset::getPresetValue(const String &targetControlAddress)
{
  for (auto &pv : presetValues)
  {

				if (pv->paramControlAddress == targetControlAddress) return pv->presetValue;
  }

  return var();
}


var PresetManager::Preset::getJSONData()
{
  var data(new DynamicObject());
  data.getDynamicObject()->setProperty("name", name);
  data.getDynamicObject()->setProperty("filter", filter);
  var presetValuesData;

  for (auto &pv : presetValues)
  {
				var pvData(new DynamicObject());
				pvData.getDynamicObject()->setProperty(ControllableContainer::controlAddressIdentifier, pv->paramControlAddress);
				pvData.getDynamicObject()->setProperty("value", pv->presetValue);
				presetValuesData.append(pvData);
  }

  data.getDynamicObject()->setProperty("values", presetValuesData);
  return data;
}

void PresetManager::Preset::loadJSONData(var data)
{

  Array<var> * pvDatas = data.getDynamicObject()->getProperty("values").getArray();

  for (auto &pvData : *pvDatas)
  {
				addPresetValue(pvData.getProperty(ControllableContainer::controlAddressIdentifier, var()),pvData.getProperty("value",var()));
  }
}
