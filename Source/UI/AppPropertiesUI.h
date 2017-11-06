/*
  ==============================================================================

    AppPropertiesUI.h
    Created: 2 Oct 2017 2:00:38pm
    Author:  Martin Hermant

  ==============================================================================
*/

#pragma once

#include "../JuceHeaderUI.h"

class PrefPanel;
class AppPropertiesUI : public ResizableWindow{
public:

    juce_DeclareSingleton(AppPropertiesUI, false);
    AppPropertiesUI();
    ~AppPropertiesUI();


    static String GeneralPageName;
    static String AudioPageName,AdvancedPageName,PluginsPageName;

    static void showAppSettings(const String & name="");
    static void closeAppSettings();


private:
    bool keyPressed (const KeyPress& key)override;
    void userTriedToCloseWindow() override;
    int getDesktopWindowStyleFlags() const override;

    ScopedPointer<PrefPanel> prefPanel;


};
