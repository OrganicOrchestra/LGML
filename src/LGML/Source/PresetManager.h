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

class PresetManager
{
public:
	class PresetValue
	{
	public:
		PresetValue(const String &_controlAddress, var _value) : paramControlAddress(_controlAddress), presetValue(_value) {}
		String paramControlAddress;
		var presetValue;
	};

	class Preset
	{
	public:
		enum Type
		{
			Node,
			Controller,
			VST
		};

		Preset(Preset::Type _type, const String & _name) : type(_type), name(_name) {}

		Preset::Type type;
		String name;
		OwnedArray<PresetValue> presetValues;

		void addPresetValue(const String &controlAddress, var value)
		{
			presetValues.add(new PresetValue(controlAddress, value));
		}

		void addPresetValues(Array<PresetValue *> _presetValues)
		{
			presetValues.addArray(_presetValues);
		}
	};

	juce_DeclareSingleton(PresetManager,true)

	OwnedArray<Preset> presets;

	void addPreset(const String &name, Preset::Type presetType, ControllableContainer * container, bool recursive = false, bool includeNotExposed = false);
	ComboBox * getPresetSelector(Preset::Type presetType);

	Preset * getPreset(Preset::Type presetType, const String &name);
};


#endif  // PRESETMANAGER_H_INCLUDED
