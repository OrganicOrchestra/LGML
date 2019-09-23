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

#include "VSTManager.h"
//#include "../Engine.h"

extern ApplicationProperties* getAppProperties();

juce_ImplementSingleton (VSTManager);

static String pluginListKey("pluginList");
VSTManager::VSTManager()
{
    formatManager.addDefaultFormats();
    auto appProps = getAppProperties();

    std::unique_ptr<XmlElement> savedPluginList = appProps->getUserSettings()->getXmlValue (pluginListKey);

    if (savedPluginList != nullptr)
        knownPluginList.recreateFromXml (*savedPluginList);

    pluginSortMethod = (KnownPluginList::SortMethod) appProps->getUserSettings()
                       ->getIntValue ("pluginSortMethod", KnownPluginList::sortByManufacturer);


    knownPluginList.addChangeListener (this);
}
VSTManager::~VSTManager()
{
    knownPluginList.removeChangeListener (this);
}

void VSTManager::changeListenerCallback (ChangeBroadcaster* changed)
{
    if (changed == &knownPluginList)
    {
        //        menuItemsChanged();

        // save the plugin list every time it gets chnaged, so that if we're scanning
        // and it crashes, we've still saved the previous ones
        std::unique_ptr<XmlElement> savedPluginList (knownPluginList.createXml());

        if (savedPluginList != nullptr)
        {
            getAppProperties()->getUserSettings()->setValue (pluginListKey, savedPluginList.get());
            getAppProperties()->getUserSettings()->saveIfNeeded();


        }
    }
}

