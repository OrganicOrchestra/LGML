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

#ifndef VSTMANAGER_H_INCLUDED
#define VSTMANAGER_H_INCLUDED

#include "../JuceHeaderAudio.h"//keep
#include <juce_audio_processors/juce_audio_processors.h>


ApplicationProperties * getAppProperties();


class VSTManager : public ChangeListener
{
public:
    juce_DeclareSingleton (VSTManager, false);
    VSTManager();
    ~VSTManager();



    void changeListenerCallback (ChangeBroadcaster* changed)override;


    AudioPluginFormatManager formatManager;
    KnownPluginList knownPluginList;
    KnownPluginList::SortMethod pluginSortMethod;
};

#endif  // VSTMANAGER_H_INCLUDED
