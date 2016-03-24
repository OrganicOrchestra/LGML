/*
 ==============================================================================
 
 VSTManager.cpp
 Created: 24 Mar 2016 3:46:31pm
 Author:  Martin Hermant
 
 ==============================================================================
 */

#include "VSTManager.h"


juce_ImplementSingleton(VSTManager);


VSTManager::VSTManager(){
    formatManager.addDefaultFormats();
    
    
    ScopedPointer<XmlElement> savedPluginList (getAppProperties().getUserSettings()->getXmlValue ("pluginList"));
    
    if (savedPluginList != nullptr)
        knownPluginList.recreateFromXml (*savedPluginList);
    
    pluginSortMethod = (KnownPluginList::SortMethod) getAppProperties().getUserSettings()
    ->getIntValue ("pluginSortMethod", KnownPluginList::sortByManufacturer);
    
    
    knownPluginList.addChangeListener (this);
}
VSTManager::~VSTManager(){
        knownPluginList.removeChangeListener (this);
}

void VSTManager::changeListenerCallback (ChangeBroadcaster* changed)
{
    if (changed == &knownPluginList)
    {
//        menuItemsChanged();
        
        // save the plugin list every time it gets chnaged, so that if we're scanning
        // and it crashes, we've still saved the previous ones
        ScopedPointer<XmlElement> savedPluginList (knownPluginList.createXml());
        
        if (savedPluginList != nullptr)
        {
            getAppProperties().getUserSettings()->setValue ("pluginList", savedPluginList);
            getAppProperties().saveIfNeeded();
        }
    }
}
void VSTManager::createSettingsWindowIfNeeded(){
    if (pluginListWindow == nullptr)
        pluginListWindow = new PluginListWindow (*this, formatManager);
    pluginListWindow->toFront (true);
}