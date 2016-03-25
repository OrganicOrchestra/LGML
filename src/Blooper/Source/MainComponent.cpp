/*
  ==============================================================================

    This file was auto-generated!

  ==============================================================================
*/

#ifndef MAINCOMPONENT_H_INCLUDED
#define MAINCOMPONENT_H_INCLUDED

#include "JuceHeader.h"

#include "NodeManager.h"
#include "NodeManagerUI.h"

#include "ControlManager.h"
#include "ControllerManagerUI.h"

#include "UIHelpers.h"
#include "TimeManagerUI.h"


// called hack because we need to fine a better place for the code in this define
#define HACK_DEVICEINPUTCHOOSE 1

//==============================================================================
/*
    This component lives inside our window, and this is where you should put all
    your controls and content.
*/

#if HACK_DEVICEINPUTCHOOSE
//==TODO implement IDs ============================================================================
namespace CommandIDs
{
    static const int open                   = 0x30000;
    static const int save                   = 0x30001;
    static const int saveAs                 = 0x30002;
    static const int newFile                = 0x30003;
    static const int showPluginListEditor   = 0x30100;
    static const int showAudioSettings      = 0x30200;
    static const int aboutBox               = 0x30300;
    static const int allWindowsForward      = 0x30400;
    static const int toggleDoublePrecision  = 0x30500;
}

#endif


ApplicationCommandManager& getCommandManager();
ApplicationProperties& getAppProperties();
AudioDeviceManager& getAudioDeviceManager();


class MainContentComponent   : public Component
#if HACK_DEVICEINPUTCHOOSE
,public ApplicationCommandTarget,public MenuBarModel
#endif
{
public:
		
	TooltipWindow tooltipWindow; // to add tooltips to an application, you
								 // just need to create one of these and leave it
								 // there to do its work..

	ScopedPointer<NodeManagerUI> nodeManagerUI;
    ScopedPointer<TimeManagerUI> timeManagerUI;
    

	ScopedPointer<ControllerManager> controllerManager;
	
	ScopedPointer<ControllerManagerViewport> controllerManagerViewport;


    // Audio

    AudioProcessorPlayer graphPlayer;
    //==============================================================================
    MainContentComponent()
	{
		DBG("Application Start");

		controllerManager = new ControllerManager();

		initAudio();

		timeManagerUI = new TimeManagerUI();
		nodeManagerUI = new NodeManagerUI(NodeManager::getInstance());


		addAndMakeVisible(timeManagerUI);
		addAndMakeVisible(nodeManagerUI);

		controllerManagerViewport = new ControllerManagerViewport(controllerManager);
		addAndMakeVisible(controllerManagerViewport);

		nodeManagerUI->setSize(getWidth(), getHeight());

		// resize after contentCreated
		setSize(1200, 600);
        
#if HACK_DEVICEINPUTCHOOSE
        
        (&getCommandManager())->registerAllCommandsForTarget (this);

#if JUCE_MAC
        setMacMainMenu (this);
#else
        setMenuBar (this);
#endif
#endif
        
        populateDefaultNodes();
	}


    ~MainContentComponent()
    {
        stopAudio();
		TimeManager::deleteInstance(); //TO PREVENT LEAK OF SINGLETON
		NodeManager::deleteInstance();
        VSTManager::deleteInstance();
        
#if HACK_DEVICEINPUTCHOOSE
#if JUCE_MAC
        setMacMainMenu (nullptr);
#else
        setMenuBar (nullptr);
#endif
#endif
    }
    
     
    void initAudio(){

        graphPlayer.setProcessor(&NodeManager::getInstance()->audioGraph);
        
        ScopedPointer<XmlElement> savedAudioState (getAppProperties().getUserSettings()
                                                   ->getXmlValue ("audioDeviceState"));
        getAudioDeviceManager().initialise (256, 256, savedAudioState, true);
        getAudioDeviceManager().addAudioCallback (&graphPlayer);
        getAudioDeviceManager().addAudioCallback(TimeManager::getInstance());
    }
    void stopAudio(){
        getAudioDeviceManager().removeAudioCallback (&graphPlayer);
        getAudioDeviceManager().removeAudioCallback(TimeManager::getInstance());
        getAudioDeviceManager().closeAudioDevice();
    }

    void populateDefaultNodes(){
        NodeBase * node = NodeManager::getInstance()->addNode(NodeFactory::NodeType::AudioIn);
        nodeManagerUI->getUIForNode(node)->setTopLeftPosition(0, 0);
        node = NodeManager::getInstance()->addNode(NodeFactory::NodeType::AudioOut);
        nodeManagerUI->getUIForNode(node)->setTopRightPosition(nodeManagerUI->getWidth(), nodeManagerUI->getHeight() - 100);
    }
    //=======================================================================
    void paint (Graphics& g) override
    {
        // (Our component is opaque, so we must completely fill the background with a solid colour)
        g.fillAll (Colours::black);


        // You can add your drawing code here!
    }
    
    
    // from here are the method for native bar display and dispatch
    // TODO figure out where to put it so that it has easy access to all functions and don't fuck up this file readability
    // PS See Juce audio host demo for inspiration of these next lines
#if HACK_DEVICEINPUTCHOOSE
    void showAudioSettings()
    {
        AudioDeviceSelectorComponent audioSettingsComp (getAudioDeviceManager(),
                                                        0, 256,
                                                        0, 256,
                                                        true, true, true, false);
        
        audioSettingsComp.setSize (500, 450);
        
        DialogWindow::LaunchOptions o;
        o.content.setNonOwned (&audioSettingsComp);
        o.dialogTitle                   = "Audio Settings";
        o.componentToCentreAround       = this;
        o.dialogBackgroundColour        = Colours::azure;
        o.escapeKeyTriggersCloseButton  = true;
        o.useNativeTitleBar             = false;
        o.resizable                     = false;
        
        o.runModal();
        
        ScopedPointer<XmlElement> audioState (getAudioDeviceManager().createStateXml());
        
        getAppProperties().getUserSettings()->setValue ("audioDeviceState", audioState);
        getAppProperties().getUserSettings()->saveIfNeeded();

    }


     ApplicationCommandTarget* getNextCommandTarget() override {return findFirstTargetParentComponent();}
    
    void getAllCommands (Array<CommandID>& commands) override{
        // this returns the set of all commands that this target can perform..
        const CommandID ids[] = {
            CommandIDs::newFile,
            CommandIDs::open,
            CommandIDs::save,
            CommandIDs::saveAs,
            CommandIDs::showPluginListEditor,
            CommandIDs::showAudioSettings,
            CommandIDs::toggleDoublePrecision,
            CommandIDs::aboutBox,
            CommandIDs::allWindowsForward
        };
        
        commands.addArray (ids, numElementsInArray (ids));
    }
    

    virtual void getCommandInfo (CommandID commandID, ApplicationCommandInfo& result) override {
        const String category ("General");
        
        switch (commandID)
        {
            case CommandIDs::newFile:
                result.setInfo ("New", "Creates a new filter graph file", category, 0);
                result.defaultKeypresses.add(KeyPress('n', ModifierKeys::commandModifier, 0));
                break;
                
            case CommandIDs::open:
                result.setInfo ("Open...", "Opens a filter graph file", category, 0);
                result.defaultKeypresses.add (KeyPress ('o', ModifierKeys::commandModifier, 0));
                break;
                
            case CommandIDs::save:
                result.setInfo ("Save", "Saves the current graph to a file", category, 0);
                result.defaultKeypresses.add (KeyPress ('s', ModifierKeys::commandModifier, 0));
                break;
                
            case CommandIDs::saveAs:
                result.setInfo ("Save As...",
                                "Saves a copy of the current graph to a file",
                                category, 0);
                result.defaultKeypresses.add (KeyPress ('s', ModifierKeys::shiftModifier | ModifierKeys::commandModifier, 0));
                break;
                
            case CommandIDs::showPluginListEditor:
                result.setInfo ("Edit the list of available plug-Ins...", String::empty, category, 0);
                result.addDefaultKeypress ('p', ModifierKeys::commandModifier);
                break;
                
            case CommandIDs::showAudioSettings:
                result.setInfo ("Change the audio device settings", String::empty, category, 0);
                result.addDefaultKeypress ('a', ModifierKeys::commandModifier);
                break;
                
            case CommandIDs::toggleDoublePrecision:
                result.setInfo ("toggles doublePrecision", String::empty, category, 0);

//                updatePrecisionMenuItem (result);
                break;
                
            case CommandIDs::aboutBox:
                result.setInfo ("About...", String::empty, category, 0);
                break;
                
            case CommandIDs::allWindowsForward:
                result.setInfo ("All Windows Forward", "Bring all plug-in windows forward", category, 0);
                result.addDefaultKeypress ('w', ModifierKeys::commandModifier);
                break;
                
            default:
                break;
        }
    }

    
    virtual bool perform (const InvocationInfo& info) override {
//        GraphDocumentComponent* const graphEditor = getGraphEditor();
        
        switch (info.commandID)
        {
                
                // TODOs
            case CommandIDs::newFile:
//                if (graphEditor != nullptr && graphEditor->graph.saveIfNeededAndUserAgrees() == FileBasedDocument::savedOk)
//                    graphEditor->graph.newDocument();
                break;
                
            case CommandIDs::open:
//                if (graphEditor != nullptr && graphEditor->graph.saveIfNeededAndUserAgrees() == FileBasedDocument::savedOk)
//                    graphEditor->graph.loadFromUserSpecifiedFile (true);
                break;
                
            case CommandIDs::save:
//                if (graphEditor != nullptr)
//                    graphEditor->graph.save (true, true);
                break;
                
            case CommandIDs::saveAs:
//                if (graphEditor != nullptr)
//                    graphEditor->graph.saveAs (File::nonexistent, true, true, true);
                break;
                
            case CommandIDs::toggleDoublePrecision:
                break;
                
                
            case CommandIDs::showPluginListEditor:
                VSTManager::getInstance()->createPluginListWindowIfNeeded();
                break;

            case CommandIDs::showAudioSettings:
                showAudioSettings();
                break;

                
            case CommandIDs::allWindowsForward:
            {
                Desktop& desktop = Desktop::getInstance();
                
                for (int i = 0; i < desktop.getNumComponents(); ++i)
                    desktop.getComponent (i)->toBehind (this);
                
                break;
            }
                
            default:
                return false;
        }
        
        return true;
    }
    
    //==============================================================================
    /** This method must return a list of the names of the menus. */
    StringArray getMenuBarNames() override {
        const char* const names[] = { "File", "Plugins", "Options", "Windows", nullptr };
        
        return StringArray (names);
    }
    
    virtual PopupMenu getMenuForIndex (int topLevelMenuIndex,
                                       const String& menuName) override{
        PopupMenu menu;
        
        if (topLevelMenuIndex == 0)
        {
            // "File" menu
            menu.addCommandItem (&getCommandManager(), CommandIDs::newFile);
            menu.addCommandItem (&getCommandManager(), CommandIDs::open);
            
            RecentlyOpenedFilesList recentFiles;
            recentFiles.restoreFromString (getAppProperties().getUserSettings()
                                           ->getValue ("recentFilterGraphFiles"));
            
            PopupMenu recentFilesMenu;
            recentFiles.createPopupMenuItems (recentFilesMenu, 100, true, true);
            menu.addSubMenu ("Open recent file", recentFilesMenu);
            
            menu.addCommandItem (&getCommandManager(), CommandIDs::save);
            menu.addCommandItem (&getCommandManager(), CommandIDs::saveAs);
            menu.addSeparator();
            menu.addCommandItem (&getCommandManager(), StandardApplicationCommandIDs::quit);
        }
        else if (topLevelMenuIndex == 1)
        {
            // "Plugins" menu
            PopupMenu pluginsMenu;
//            addPluginsToMenu (pluginsMenu);
            menu.addSubMenu ("Create plugin", pluginsMenu);
            menu.addSeparator();
            menu.addItem (250, "Delete all plugins");
        }
        else if (topLevelMenuIndex == 2)
        {
            // "Options" menu
            
            menu.addCommandItem (&getCommandManager(), CommandIDs::showPluginListEditor);
            menu.addCommandItem (&getCommandManager(), CommandIDs::showAudioSettings);
            menu.addCommandItem (&getCommandManager(), CommandIDs::toggleDoublePrecision);
            
            menu.addSeparator();
            menu.addCommandItem (&getCommandManager(), CommandIDs::aboutBox);
        }
        else if (topLevelMenuIndex == 3)
        {
            menu.addCommandItem (&getCommandManager(), CommandIDs::allWindowsForward);
        }
        
        return menu;
    }
    
    /** This is called when a menu item has been clicked on.
     
     @param menuItemID           the item ID of the PopupMenu item that was selected
     @param topLevelMenuIndex    the index of the top-level menu from which the item was
     chosen (just in case you've used duplicate ID numbers
     on more than one of the popup menus)
     */
    void menuItemSelected (int menuItemID,int topLevelMenuIndex) override{}
    
#endif
    
    
    
    void resized() override
    {
		Rectangle<int> r = getLocalBounds();
		timeManagerUI->setBounds(r.removeFromTop(20));
		controllerManagerViewport->setBounds(r.removeFromLeft(300));
		nodeManagerUI->setBounds(r);
    }

private:
    //==============================================================================

    // Your private member variables go here...

	JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(MainContentComponent)
};


// (This function is called by the app startup code to create our main component)
Component* createMainContentComponent()     { return new MainContentComponent(); }


#endif  // MAINCOMPONENT_H_INCLUDED
