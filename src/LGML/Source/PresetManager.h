/*
  ==============================================================================

    PresetManager.h
    Created: 25 Apr 2016 2:02:05pm
    Author:  bkupe

  ==============================================================================
*/

#ifndef PRESETMANAGER_H_INCLUDED
#define PRESETMANAGER_H_INCLUDED

#include "JuceHeader.h"

class ControllableContainer;

enum PresetChoice
{
    SaveCurrent = -3,
    SaveToNew = -2,
    ResetToDefault = -1
};

class PresetManager
{
public:
    class PresetValue
    {
    public:
        PresetValue(const String &_controlAddress, var _value) : paramControlAddress(_controlAddress), presetValue(_value) {}
        String paramControlAddress;
        var presetValue;
        JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(PresetValue)
    };

    class Preset
    {
    public:
        Preset(const String & _name, String _filter) : filter(_filter), name(_name) {}

        String filter; //Used to filter which preset to propose depending on the object (specific nodes, vst, controller, etc.)
        String name;
        OwnedArray<PresetValue> presetValues;
        int presetId; //change each time the a preset list is created, but we don't care because ControllableContainer keeps the pointer to the Preset

        void addPresetValue(const String &controlAddress, var value)
        {
            presetValues.add(new PresetValue(controlAddress, value));
        }

        void addPresetValues(Array<PresetValue *> _presetValues)
        {
            presetValues.addArray(_presetValues);
        }

		var getPresetValue(const String &targetControlAddress)
		{
			for (auto &pv : presetValues)
			{

				if (pv->paramControlAddress == targetControlAddress) return pv->presetValue;
			}

			return var();
		}


		var getJSONData()
		{
			var data(new DynamicObject());
			data.getDynamicObject()->setProperty("name", name);
			data.getDynamicObject()->setProperty("filter", filter);
			var presetValuesData;

			for (auto &pv : presetValues)
			{
				var pvData(new DynamicObject());
				pvData.getDynamicObject()->setProperty("controlAddress", pv->paramControlAddress);
				pvData.getDynamicObject()->setProperty("value", pv->presetValue);
				presetValuesData.append(pvData);
			}

			data.getDynamicObject()->setProperty("values", presetValuesData);
			return data;
		}

		void loadJSONData(var data)
		{

			Array<var> * pvDatas = data.getDynamicObject()->getProperty("values").getArray();

			for (auto &pvData : *pvDatas)
			{
				addPresetValue(pvData.getProperty("controlAddress", var()),pvData.getProperty("value",var()));
			}
		}

        JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(Preset)
    };

    juce_DeclareSingleton(PresetManager,true)

    OwnedArray<Preset> presets;

    PresetManager();
    virtual ~PresetManager();

    Preset * addPresetFromControllableContainer(const String &name, const String & filter, ControllableContainer * container, bool recursive = false, bool includeNotExposed = false);
    ComboBox * getPresetSelector(String filter);
    Preset * getPreset(String filter, const String &name);
    void fillWithPresets(ComboBox * cb, String filter);

    void clear();


	var getJSONData();
	void loadJSONData(var data);

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(PresetManager)
};


#endif  // PRESETMANAGER_H_INCLUDED
