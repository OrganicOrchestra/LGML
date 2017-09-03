/* Copyright © Organic Orchestra, 2017
*
* This file is part of LGML.  LGML is a software to manipulate sound in realtime
*
* This program is free software; you can redistribute it and/or modify
* it under the terms of the GNU General Public License as published by
* the Free Software Foundation (version 3 of the License).
*
* This program is distributed in the hope that it will be useful,
* but WITHOUT ANY WARRANTY; without even the implied warranty of
* MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
*
*/


#ifndef MAINCOMPONENT_H_INCLUDED
#define MAINCOMPONENT_H_INCLUDED

#pragma warning( disable : 4244 4100)


/*
#include "NodeManager.h"
#include "NodeManagerUI.h"

#include "ControlManager.h"
#include "ControllerManagerUI.h"
#include "TimeManagerUI.h"

#include "RuleManager.h"
#include "RuleManagerUI.h"

#include "FastMapper.h"
#include "FastMapperUI.h"

#include "LGMLLoggerUI.h"
*/


//==============================================================================
/*
 This component lives inside our window, and this is where you should put all
 your controls and content.
 */



#include "Engine.h"
#include "ShapeShifterManager.h"//keep
#include "DebugHelpers.h"//keep
#include "ProgressWindow.h"

ApplicationCommandManager& getCommandManager();
ApplicationProperties& getAppProperties();
AudioDeviceManager& getAudioDeviceManager();

class MainContentComponent   : public juce::Component,public ApplicationCommandTarget,public MenuBarModel,public Engine::EngineListener,private Timer

{
public:

    TooltipWindow tooltipWindow; // to add tooltips to an application, you
    // just need to create one of these and leave it
    // there to do its work..
    Engine * engine;

	ScopedPointer<ProgressWindow> fileProgressWindow;

	// from engineListener
	void startLoadFile() override;
	void fileProgress(float percent, int state)override;
	void endLoadFile() override;


	void timerCallback() override;
    ScopedPointer<LookAndFeel> lookAndFeelOO;

    //==============================================================================
    MainContentComponent(Engine * e);
    ~MainContentComponent();

    //==============================================================================
    // see MainComponent.cpp

    //==============================================================================
  void paint (Graphics& g) override;

	void paintOverChildren(Graphics & g) override;
    void resized() override;


    void showAudioSettings();


    //==============================================================================
    // see MainComponentCommands.cpp

    // inherited from MenuBarModel , ApplicationCommandTarget
    ApplicationCommandTarget* getNextCommandTarget() override {return findFirstTargetParentComponent();}
    void getAllCommands (Array<CommandID>& commands) override;
    virtual void getCommandInfo (CommandID commandID, ApplicationCommandInfo& result) override ;
    virtual bool perform (const InvocationInfo& info) override ;
    StringArray getMenuBarNames() override ;
    virtual PopupMenu getMenuForIndex (int topLevelMenuIndex,const String& menuName) override;
	void menuItemSelected(int /*menuItemID*/, int /*topLevelMenuIndex*/) override;


    void updateStimulateAudioItem (ApplicationCommandInfo& info);
	void focusGained(FocusChangeType cause)override;

private:
    //==============================================================================

    // Your private member variables go here...

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(MainContentComponent)
};






#endif  // MAINCOMPONENT_H_INCLUDED
