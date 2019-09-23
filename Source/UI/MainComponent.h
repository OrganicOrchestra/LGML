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

#ifndef MAINCOMPONENT_H_INCLUDED
#define MAINCOMPONENT_H_INCLUDED

#pragma warning( disable : 4244 4100)


//==============================================================================
/*
 This component lives inside our window, and this is where you should put all
 your controls and content.
 */



#include "../Engine.h" //EngineListener
#include "ShapeShifter/ShapeShifterManager.h"
#include "../Utils/DebugHelpers.h"
#include "ProgressWindow.h"


class UndoWatcher;
ApplicationCommandManager& getCommandManager();
ApplicationProperties * getAppProperties();
UndoManager & getAppUndoManager();
AudioDeviceManager& getAudioDeviceManager();

class MainContentComponent   : public juce::Component, public ApplicationCommandTarget, public MenuBarModel, public Engine::EngineListener, private Timer

{
public:


    Engine* engine;

    std::unique_ptr<ProgressWindow> fileProgressWindow;

    // from engineListener
    void startLoadFile() override;
    void fileProgress (float percent, int state)override;
    void endLoadFile() override;


    void timerCallback() override;


    //==============================================================================
    MainContentComponent (Engine* e);
    ~MainContentComponent();

    //==============================================================================
    // see MainComponent.cpp

    //==============================================================================
    void paint (Graphics& g) override;

    void paintOverChildren (Graphics& g) override;
    void resized() override;



    //==============================================================================
    // see MainComponentCommands.cpp

    // inherited from MenuBarModel , ApplicationCommandTarget
    ApplicationCommandTarget* getNextCommandTarget() override {return findFirstTargetParentComponent();}
    void getAllCommands (Array<CommandID>& commands) override;
    virtual void getCommandInfo (CommandID commandID, ApplicationCommandInfo& result) override ;
    virtual bool perform (const InvocationInfo& info) override ;
    StringArray getMenuBarNames() override ;
    virtual PopupMenu getMenuForIndex (int topLevelMenuIndex, const String& menuName) override;
    void menuItemSelected (int /*menuItemID*/, int /*topLevelMenuIndex*/) override;


    
    void focusGained (FocusChangeType cause)override;
    
private:
    //==============================================================================

    // Your private member variables go here...
    
    TooltipWindow tooltipWindow;


    std::unique_ptr<UndoWatcher> undoWatcher;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (MainContentComponent)
};






#endif  // MAINCOMPONENT_H_INCLUDED
