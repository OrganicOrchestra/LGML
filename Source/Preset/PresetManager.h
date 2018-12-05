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

#ifndef PRESETMANAGER_H_INCLUDED
#define PRESETMANAGER_H_INCLUDED

#include "../JuceHeaderCore.h"//keep
class ParameterContainer;



class PresetManager
{
public:
    class PresetValue
    {
    public:
        PresetValue (const String& _controlAddress, var _value) : paramControlAddress (_controlAddress), presetValue (_value) {}
        String paramControlAddress;
        var presetValue;
        JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (PresetValue)
    };

    class Preset
    {
    public:
        Preset (const String& _name, String _filter) : filter (_filter), name (_name) {}

        String filter; //Used to filter which preset to propose depending on the object (specific nodes, vst, controller, etc.)
        String name;
        OwnedArray<PresetValue> presetValues;
        int presetId; //change each time the a preset list is created, but we don't care because ControllableContainer keeps the pointer to the Preset

        void addPresetValue (const String& controlAddress, var value);
        void addPresetValues (Array<PresetValue*> _presetValues);
        var getPresetValue (const String& targetControlAddress);

        void clear();

        DynamicObject*   createObject();

        void configureFromObject (DynamicObject* data);

        JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (Preset)
    };

    juce_DeclareSingleton (PresetManager, true)

    OwnedArray<Preset> presets;

    PresetManager();
    virtual ~PresetManager();

    Preset* addPresetFromControllableContainer (const String& name, const String& filter, ParameterContainer* container, bool recursive = false, bool includeNotExposed = false);
    Preset* getPreset (String filter, const String& name) const;

    void removePresetForIdx (int idx);
    int getNumPresetForFilter (const String&) const;

    void deleteAllUnusedPresets (ParameterContainer* rootContainer);
    int deletePresetsForContainer (ParameterContainer* container, bool recursive = true); //delete all presets that no controllable uses

    void clear();


    DynamicObject* createObject();
    void configureFromObject (DynamicObject* data);

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (PresetManager)
};


#endif  // PRESETMANAGER_H_INCLUDED
