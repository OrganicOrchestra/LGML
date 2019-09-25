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

//class Preset;
#include "Preset.h"
class ParameterContainer;

class EngineSync;



class PresetManager
{
public:
    




    class Listener{
    public:
        Listener();
        virtual ~Listener();
        virtual void presetAdded(Preset * p) = 0;
        virtual void presetRemoved(Preset * p) = 0;
        virtual void presetSaved(Preset * ) {};
    };


    juce_DeclareSingleton (PresetManager, true)



    PresetManager();
    virtual ~PresetManager();

    
    Preset* getPreset (const String & filter, const String& name) const;

    Array<WeakReference<Preset> > getPresetsForFilter(String filter);
    Array<WeakReference<Preset> >  getPresetsForType(String type,ParameterContainer *ownerToIgnore=nullptr);
    void removePresetForIdx (int idx);
    int getNumPresetForFilter (const String&) const;

    void deleteAllUnusedPresets (ParameterContainer* rootContainer);
    int deletePresetsForContainer (ParameterContainer* container, bool recursive = true); //delete all presets that no controllable uses

    void clear();

    
    Preset * addPreset(Preset * );
    void removePreset(Preset * );

    void notifyPresetSaved(Listener* notif,Preset *p){presetListeners.callExcluding(notif,&Listener::presetSaved,p);}
    



    ListenerList<Listener> presetListeners;
    
private:


    OwnedArray<Preset> presets;

    std::unique_ptr<EngineSync> engineSync;
    friend class EngineSync;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (PresetManager)
};


#endif  // PRESETMANAGER_H_INCLUDED
