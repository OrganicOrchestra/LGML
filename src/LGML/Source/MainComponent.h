/*
 ==============================================================================

 This file was auto-generated!

 ==============================================================================
 */

#ifndef MAINCOMPONENT_H_INCLUDED
#define MAINCOMPONENT_H_INCLUDED



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

#include "LookAndFeelOO.h"
//==============================================================================
/*
 This component lives inside our window, and this is where you should put all
 your controls and content.
 */

ApplicationCommandManager& getCommandManager();
ApplicationProperties& getAppProperties();
AudioDeviceManager& getAudioDeviceManager();

#include "Engine.h"
#include "ShapeShifterManager.h"//keep
#include "DebugHelpers.h"//keep

class MainContentComponent   : public Component,public ApplicationCommandTarget,public MenuBarModel,public Engine::EngineListener,public Timer

{
public:

    TooltipWindow tooltipWindow; // to add tooltips to an application, you
    // just need to create one of these and leave it
    // there to do its work..


	/*
	ScopedPointer<TimeManagerUI> timeManagerUI;
	ScopedPointer<InspectorViewport> inspectorViewport;

	ScopedPointer<NodeManagerUIViewport> nodeManagerViewport;
	ScopedPointer<ControllerManagerUI> controllerManagerUI;
	ScopedPointer<RuleManagerUI> ruleManagerUI;
	ScopedPointer<FastMapperViewport> fastMapperViewport;

    ScopedPointer<LGMLLoggerUI> lgmlLoggerUI;
	*/

    Engine * engine;
  // from engineListener
  void startLoadFile() override;
  void fileProgress(float percent, int state)override;
  void endLoadFile() override;


  void timerCallback() override;
    ScopedPointer<LookAndFeelOO> lookAndFeelOO;

    //==============================================================================
    MainContentComponent(Engine * e);
    ~MainContentComponent();

    //==============================================================================
    // see MainComponent.cpp

    //==============================================================================
    void paint (Graphics& g) override{
		g.fillAll (BG_COLOR.darker());
	}
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
