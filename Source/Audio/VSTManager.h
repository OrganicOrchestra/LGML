/*
 ==============================================================================

 Copyright © Organic Orchestra, 2017

 This file is part of LGML. LGML is a software to manipulate sound in realtime

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


ApplicationCommandManager& getCommandManager();
ApplicationProperties& getAppProperties();


class VSTManager : public ChangeListener
{
public:
    juce_DeclareSingleton (VSTManager, false);
    VSTManager();
    ~VSTManager();



    void changeListenerCallback (ChangeBroadcaster* changed)override;
    void createPluginListWindowIfNeeded();


    class PluginListWindow  : public DocumentWindow
    {
    public:
        PluginListWindow (VSTManager& owner_,
                          AudioPluginFormatManager& pluginFormatManager)
            : DocumentWindow ("Available Plugins", Colours::white,
                              DocumentWindow::minimiseButton | DocumentWindow::closeButton)
            , owner (owner_)
        {
            const File deadMansPedalFile (getAppProperties().getUserSettings()
                                          ->getFile().getSiblingFile ("RecentlyCrashedPluginsList"));

            setContentOwned (new PluginListComponent (pluginFormatManager,
                                                      owner.knownPluginList,
                                                      deadMansPedalFile,
                                                      getAppProperties().getUserSettings()), true);

            setResizable (true, false);
            setResizeLimits (300, 400, 800, 1500);
            setTopLeftPosition (60, 60);

            restoreWindowStateFromString (getAppProperties().getUserSettings()->getValue ("listWindowPos"));
            setVisible (true);
        }

        ~PluginListWindow()
        {
            getAppProperties().getUserSettings()->setValue ("listWindowPos", getWindowStateAsString());
            clearContentComponent();
        }

        void closeButtonPressed() {owner.pluginListWindow = nullptr;}

    private:
        VSTManager& owner;

        JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (PluginListWindow)
    };




    ScopedPointer<PluginListWindow> pluginListWindow;
    AudioPluginFormatManager formatManager;
    KnownPluginList knownPluginList;
    KnownPluginList::SortMethod pluginSortMethod;
};

#endif  // VSTMANAGER_H_INCLUDED
