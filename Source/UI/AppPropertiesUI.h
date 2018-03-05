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
