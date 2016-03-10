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

#define HACK_DEVICEINPUTCHOOSE 1

//==============================================================================
/*
    This component lives inside our window, and this is where you should put all
    your controls and content.
*/

#if HACK_DEVICEINPUTCHOOSE
//==TODO put in Header ============================================================================
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
    AudioDeviceManager deviceManager;
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
	}


    ~MainContentComponent()
    {
        stopAudio();
		TimeManager::deleteInstance(); //TO PREVENT LEAK OF SINGLETON
		NodeManager::deleteInstance();
        
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
        deviceManager.initialise (256, 256, savedAudioState, true);
        deviceManager.addAudioCallback (&graphPlayer);
        deviceManager.addAudioCallback(TimeManager::getInstance());
    }
    void stopAudio(){
        deviceManager.removeAudioCallback (&graphPlayer);
        deviceManager.removeAudioCallback(TimeManager::getInstance());
        deviceManager.closeAudioDevice();
    }

    //=======================================================================
    void paint (Graphics& g) override
    {
        // (Our component is opaque, so we must completely fill the background with a solid colour)
        g.fillAll (Colours::black);


        // You can add your drawing code here!
    }
#if HACK_DEVICEINPUTCHOOSE
    void showAudioSettings()
    {
        AudioDeviceSelectorComponent audioSettingsComp (deviceManager,
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
        
        ScopedPointer<XmlElement> audioState (deviceManager.createStateXml());
        
        getAppProperties().getUserSettings()->setValue ("audioDeviceState", audioState);
        getAppProperties().getUserSettings()->saveIfNeeded();
        
        //            GraphDocumentComponent* const graphEditor = getGraphEditor();
        //
        //            if (graphEditor != nullptr)
        //                graphEditor->graph.removeIllegalConnections();
    }


     ApplicationCommandTarget* getNextCommandTarget() {
            return findFirstTargetParentComponent();
    }
    
    void getAllCommands (Array<CommandID>& commands) {
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

    
    virtual bool perform (const InvocationInfo& info) {
//        GraphDocumentComponent* const graphEditor = getGraphEditor();
        
        switch (info.commandID)
        {
//            case CommandIDs::newFile:
//                if (graphEditor != nullptr && graphEditor->graph.saveIfNeededAndUserAgrees() == FileBasedDocument::savedOk)
//                    graphEditor->graph.newDocument();
//                break;
//                
//            case CommandIDs::open:
//                if (graphEditor != nullptr && graphEditor->graph.saveIfNeededAndUserAgrees() == FileBasedDocument::savedOk)
//                    graphEditor->graph.loadFromUserSpecifiedFile (true);
//                break;
//                
//            case CommandIDs::save:
//                if (graphEditor != nullptr)
//                    graphEditor->graph.save (true, true);
//                break;
//                
//            case CommandIDs::saveAs:
//                if (graphEditor != nullptr)
//                    graphEditor->graph.saveAs (File::nonexistent, true, true, true);
//                break;
//                
//            case CommandIDs::showPluginListEditor:
//                if (pluginListWindow == nullptr)
//                    pluginListWindow = new PluginListWindow (*this, formatManager);
//                
//                pluginListWindow->toFront (true);
//                break;
//                
            case CommandIDs::showAudioSettings:
                showAudioSettings();
                break;
//                
//            case CommandIDs::toggleDoublePrecision:
//                if (PropertiesFile* props = getAppProperties().getUserSettings())
//                {
//                    bool newIsDoublePrecision = ! isDoublePrecisionProcessing();
//                    props->setValue ("doublePrecisionProcessing", var (newIsDoublePrecision));
//                    
//                    {
//                        ApplicationCommandInfo cmdInfo (info.commandID);
//                        updatePrecisionMenuItem (cmdInfo);
//                        menuItemsChanged();
//                    }
//                    
//                    if (graphEditor != nullptr)
//                        graphEditor->setDoublePrecision (newIsDoublePrecision);
//                }
//                break;
                
//            case CommandIDs::aboutBox:
//                // TODO
//                break;
//                
//            case CommandIDs::allWindowsForward:
//            {
//                Desktop& desktop = Desktop::getInstance();
//                
//                for (int i = 0; i < desktop.getNumComponents(); ++i)
//                    desktop.getComponent (i)->toBehind (this);
//                
//                break;
//            }
                
            default:
                return false;
        }
        
        return true;
    }
    
    //==============================================================================
    /** This method must return a list of the names of the menus. */
    StringArray getMenuBarNames() {
        const char* const names[] = { "File", "Plugins", "Options", "Windows", nullptr };
        
        return StringArray (names);
    }
    
    /** This should return the popup menu to display for a given top-level menu.
     
     @param topLevelMenuIndex    the index of the top-level menu to show
     @param menuName             the name of the top-level menu item to show
     */
    virtual PopupMenu getMenuForIndex (int topLevelMenuIndex,
                                       const String& menuName) {
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
            
//            PopupMenu sortTypeMenu;
//            sortTypeMenu.addItem (200, "List plugins in default order",      true, pluginSortMethod == KnownPluginList::defaultOrder);
//            sortTypeMenu.addItem (201, "List plugins in alphabetical order", true, pluginSortMethod == KnownPluginList::sortAlphabetically);
//            sortTypeMenu.addItem (202, "List plugins by category",           true, pluginSortMethod == KnownPluginList::sortByCategory);
//            sortTypeMenu.addItem (203, "List plugins by manufacturer",       true, pluginSortMethod == KnownPluginList::sortByManufacturer);
//            sortTypeMenu.addItem (204, "List plugins based on the directory structure", true, pluginSortMethod == KnownPluginList::sortByFileSystemLocation);
//            menu.addSubMenu ("Plugin menu type", sortTypeMenu);
            
//            menu.addSeparator();
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
    void menuItemSelected (int menuItemID,
                                   int topLevelMenuIndex) {
//        GraphDocumentComponent* const graphEditor = getGraphEditor();
        
//        if (menuItemID == 250)
//        {
//            if (graphEditor != nullptr)
//                graphEditor->graph.clear();
//        }
//        else
//            if (menuItemID >= 100 && menuItemID < 200)
//        {
//            RecentlyOpenedFilesList recentFiles;
//            recentFiles.restoreFromString (getAppProperties().getUserSettings()
//                                           ->getValue ("recentFilterGraphFiles"));
//            
//            if (graphEditor != nullptr && graphEditor->graph.saveIfNeededAndUserAgrees() == FileBasedDocument::savedOk)
//                graphEditor->graph.loadFrom (recentFiles.getFile (menuItemID - 100), true);
//        }
//        else if (menuItemID >= 200 && menuItemID < 210)
//        {
//            if (menuItemID == 200)     pluginSortMethod = KnownPluginList::defaultOrder;
//            else if (menuItemID == 201)     pluginSortMethod = KnownPluginList::sortAlphabetically;
//            else if (menuItemID == 202)     pluginSortMethod = KnownPluginList::sortByCategory;
//            else if (menuItemID == 203)     pluginSortMethod = KnownPluginList::sortByManufacturer;
//            else if (menuItemID == 204)     pluginSortMethod = KnownPluginList::sortByFileSystemLocation;
//            
//            getAppProperties().getUserSettings()->setValue ("pluginSortMethod", (int) pluginSortMethod);
//            
//            menuItemsChanged();
//        }
//        else
//        {
//            createPlugin (getChosenType (menuItemID),
//                          proportionOfWidth  (0.3f + Random::getSystemRandom().nextFloat() * 0.6f),
//                          proportionOfHeight (0.3f + Random::getSystemRandom().nextFloat() * 0.6f));
//        }
    }
    
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
