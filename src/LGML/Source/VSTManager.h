/*
  ==============================================================================

    VSTManager.h
    Created: 24 Mar 2016 3:46:31pm
    Author:  Martin Hermant

  ==============================================================================
*/

#ifndef VSTMANAGER_H_INCLUDED
#define VSTMANAGER_H_INCLUDED



#include "JuceHeader.h"

ApplicationCommandManager& getCommandManager();
ApplicationProperties& getAppProperties();


class VSTManager : public ChangeListener{
public:
    juce_DeclareSingleton(VSTManager,false);
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
        ,owner (owner_)
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

        void closeButtonPressed(){owner.pluginListWindow = nullptr;}

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
