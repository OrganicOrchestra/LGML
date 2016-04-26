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

enum NodeType;

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
		int presetId; //change each time the a preset list is created, but we don't care because ControllableContainer keeps the pointer to the Preset

		void addPresetValue(const String &controlAddress, var value)
		{
			presetValues.add(new PresetValue(controlAddress, value));
		}

		void addPresetValues(Array<PresetValue *> _presetValues)
		{
			presetValues.addArray(_presetValues);
		}

		JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(Preset)
	};

	class NodePreset : public Preset
	{ 
	public:
		NodePreset(NodeType nodeType, const String &_name) : Preset(Type::Node, _name), nodeType(nodeType) {}
		NodeType nodeType;

		JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(NodePreset)
	};

	juce_DeclareSingleton(PresetManager,true)

	OwnedArray<NodePreset> nodePresets;

	PresetManager();
	virtual ~PresetManager();
	
	NodePreset * addNodePreset(const String &name, NodeType nodeType, ControllableContainer * container, bool recursive = false, bool includeNotExposed = false);
	ComboBox * getNodePresetSelector(NodeType nodeType);
	NodePreset * getNodePreset(NodeType nodeType, const String &name);
	void fillWithNodePresets(ComboBox * cb, NodeType nodeType);

	void clear();
	//JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(PresetManager)
};


#endif  // PRESETMANAGER_H_INCLUDED
