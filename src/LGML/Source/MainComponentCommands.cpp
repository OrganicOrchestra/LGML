/*
  ==============================================================================

    MainComponentMenuBar.cpp
    Created: 25 Mar 2016 6:02:02pm
    Author:  Martin Hermant

  ==============================================================================
*/


#include "MainComponent.h"


namespace CommandIDs
{
    static const int open                   = 0x30000;
    static const int save                   = 0x30001;
    static const int saveAs                 = 0x30002;
    static const int newFile                = 0x30003;
    static const int openLastDocument       = 0x30004;
    static const int playPause              = 0x30010;
    static const int showPluginListEditor   = 0x30100;
    static const int showAudioSettings      = 0x30200;
    static const int aboutBox               = 0x30300;
    static const int allWindowsForward      = 0x30400;
    static const int toggleDoublePrecision  = 0x30500;

}




void MainContentComponent::getCommandInfo (CommandID commandID, ApplicationCommandInfo& result)  {
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

        case CommandIDs::openLastDocument:
            result.setInfo("Open Last Document", "Opens a filter graph file", category, 0);
            result.defaultKeypresses.add(KeyPress('o', ModifierKeys::shiftModifier | ModifierKeys::commandModifier, 0));
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
            //updatePrecisionMenuItem (result);
            break;

        case CommandIDs::aboutBox:
            result.setInfo ("About...", String::empty, category, 0);
            break;

        case CommandIDs::allWindowsForward:
            result.setInfo ("All Windows Forward", "Bring all plug-in windows forward", category, 0);
            result.addDefaultKeypress ('w', ModifierKeys::commandModifier);
            break;
        case CommandIDs::playPause:
            result.setInfo ("Play/pause", "Play or pause LGML", category, 0);
            result.addDefaultKeypress (' ',ModifierKeys::noModifiers);
            break;


        default:
            break;
    }
}


void MainContentComponent::getAllCommands (Array<CommandID>& commands) {
    // this returns the set of all commands that this target can perform..
    const CommandID ids[] = {
        CommandIDs::newFile,
        CommandIDs::open,
        CommandIDs::openLastDocument,
        CommandIDs::save,
        CommandIDs::saveAs,
        CommandIDs::showPluginListEditor,
        CommandIDs::showAudioSettings,
        CommandIDs::toggleDoublePrecision,
        CommandIDs::aboutBox,
        CommandIDs::allWindowsForward,
        CommandIDs::playPause
    };

    commands.addArray (ids, numElementsInArray (ids));
}


PopupMenu MainContentComponent::getMenuForIndex (int /*topLevelMenuIndex*/, const String& menuName) {
    PopupMenu menu;
	
	if(menuName == "File")
	{
        // "File" menu
        menu.addCommandItem (&getCommandManager(), CommandIDs::newFile);
        menu.addCommandItem (&getCommandManager(), CommandIDs::open);
        menu.addCommandItem(&getCommandManager(), CommandIDs::openLastDocument);

        RecentlyOpenedFilesList recentFiles;
        recentFiles.restoreFromString (getAppProperties().getUserSettings()
                                       ->getValue ("recentNodeGraphFiles"));

        PopupMenu recentFilesMenu;
        recentFiles.createPopupMenuItems (recentFilesMenu, 100, true, true);
        menu.addSubMenu ("Open recent file", recentFilesMenu);

        menu.addCommandItem (&getCommandManager(), CommandIDs::save);
        menu.addCommandItem (&getCommandManager(), CommandIDs::saveAs);
        menu.addSeparator();
        menu.addCommandItem (&getCommandManager(), StandardApplicationCommandIDs::quit);

    }else if (menuName == "Plugins")
    {
        // "Plugins" menu
        PopupMenu pluginsMenu;
        //            addPluginsToMenu (pluginsMenu);
        menu.addSubMenu ("Create plugin", pluginsMenu);
        menu.addSeparator();
        menu.addItem (250, "Delete all plugins");
    } else if (menuName == "Options")
    {
        // "Options" menu

        menu.addCommandItem (&getCommandManager(), CommandIDs::showPluginListEditor);
        menu.addCommandItem (&getCommandManager(), CommandIDs::showAudioSettings);
        menu.addCommandItem (&getCommandManager(), CommandIDs::toggleDoublePrecision);

        menu.addSeparator();
        menu.addCommandItem (&getCommandManager(), CommandIDs::aboutBox);
    } else if (menuName == "Windows")
    {
        menu.addCommandItem (&getCommandManager(), CommandIDs::allWindowsForward);
	}
	else if (menuName == "Panels")
	{
		return ShapeShifterManager::getInstance()->getPanelsMenu();
	}

    return menu;
}

bool MainContentComponent::perform(const InvocationInfo& info) {

    switch (info.commandID)
    {


        case CommandIDs::newFile:
            engine->createNewGraph();

            break;

        case CommandIDs::open:
            engine->loadFromUserSpecifiedFile (true);

            break;

        case CommandIDs::openLastDocument:
            engine->loadDocument(engine->getLastDocumentOpened());
            break;

        case CommandIDs::save:
            engine->save (true, true);
            break;

        case CommandIDs::saveAs:
            engine->saveAs (File::nonexistent, true, true, true);
            break;

        case CommandIDs::toggleDoublePrecision:
            DBG("double precision not supported yet...");
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
                // doesn't work on windows
                if (desktop.getComponent(i)->getParentComponent() != nullptr)
                {
                    desktop.getComponent(i)->toBehind(this);
                }
                break;
        }
        case CommandIDs::playPause:
            TimeManager::getInstance()->togglePlay();
            break;

        default:
            return false;
    }

    return true;
}

void MainContentComponent::menuItemSelected(int menuItemID, int topLevelMenuIndex)
{
	
	String menuName = getMenuBarNames()[topLevelMenuIndex];
	if (menuName == "Panels")
	{
		ShapeShifterManager::getInstance()->handleMenuPanelCommand(menuItemID);
	}
	
}


StringArray MainContentComponent::getMenuBarNames() {
    const char* const names[] = { "File", "Plugins", "Options", "Windows","Panels", nullptr };
    return StringArray (names);
}
