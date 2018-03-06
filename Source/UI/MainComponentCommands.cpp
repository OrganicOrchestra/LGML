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

#include "MainComponent.h"
#include "../FastMapper/FastMap.h"
#include "../Engine.h"
#include "Inspector/Inspector.h"
#include "../Node/NodeContainer/NodeContainer.h"
#include "LGMLDragger.h"
#include "AppPropertiesUI.h"

#include "../Node/NodeContainer/UI/NodeContainerViewer.h"// for copy paste

namespace CommandIDs
{
static const int open                   = 0x30000;
static const int save                   = 0x30001;
static const int saveAs                 = 0x30002;
static const int newFile                = 0x30003;
static const int openLastDocument       = 0x30004;
static const int playPause              = 0x30010;
static const int copySelection          = 0x30020;
static const int cutSelection           = 0x30021;
static const int pasteSelection         = 0x30022;
static const int showPluginListEditor   = 0x30100;
static const int showAppSettings        = 0x30200;
static const int showAudioSettings      = 0x30201;
static const int aboutBox               = 0x30300;
static const int allWindowsForward      = 0x30400;
//static const int unused               = 0x30500;
//static const int stimulateCPU           = 0x30600;
static const int toggleMappingMode      = 0x30700;

// range ids
static const int lastFileStartID        = 100; // 100 to 200 max

}




void MainContentComponent::getCommandInfo (CommandID commandID, ApplicationCommandInfo& result)
{
    const String category ("General");

    switch (commandID)
    {
        case CommandIDs::newFile:
            result.setInfo ("New", "Creates a new filter graph file", category, 0);
            result.defaultKeypresses.add (KeyPress ('n', ModifierKeys::commandModifier, 0));
            break;

        case CommandIDs::open:
            result.setInfo ("Open...", "Opens a filter graph file", category, 0);
            result.defaultKeypresses.add (KeyPress ('o', ModifierKeys::commandModifier, 0));
            break;

        case CommandIDs::openLastDocument:
            result.setInfo ("Open Last Document", "Opens a filter graph file", category, 0);
            result.defaultKeypresses.add (KeyPress ('o', ModifierKeys::shiftModifier | ModifierKeys::commandModifier, 0));
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
            result.setInfo ("Plug-Ins Settings...", String::empty, category, 0);
            result.addDefaultKeypress ('p', ModifierKeys::commandModifier);
            break;

        case CommandIDs::showAudioSettings:
            result.setInfo ("Audio settings...", String::empty, category, 0);
            result.addDefaultKeypress ('a', ModifierKeys::commandModifier);
            break;
        case CommandIDs::showAppSettings:
            result.setInfo ("Settings...", String::empty, category, 0);
            result.addDefaultKeypress (',', ModifierKeys::commandModifier);
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
            result.addDefaultKeypress (' ', ModifierKeys::noModifiers);
            break;

        case CommandIDs::toggleMappingMode:
            result.setInfo ("toggle mappingMode", "toggle param mapping mode", category, 0);
            result.addDefaultKeypress ('m', ModifierKeys::commandModifier);
            break;

        case CommandIDs::copySelection:
            result.setInfo ("Copy selection", "Copy current selection", category, 0);
            result.addDefaultKeypress ('c', ModifierKeys::commandModifier);
            break;

        case CommandIDs::cutSelection:
            result.setInfo ("Cut selection", "Cut current selection", category, 0);
            result.addDefaultKeypress ('x', ModifierKeys::commandModifier);
            break;

        case CommandIDs::pasteSelection:
            result.setInfo ("Paste selection", "Paste previously copied item into selected object if possible", category, 0);
            result.addDefaultKeypress ('v', ModifierKeys::commandModifier);
            break;



        default:
            break;
    }
}


void MainContentComponent::getAllCommands (Array<CommandID>& commands)
{
    // this returns the set of all commands that this target can perform..
    const CommandID ids[] =
    {
        CommandIDs::newFile,
        CommandIDs::open,
        CommandIDs::openLastDocument,
        CommandIDs::save,
        CommandIDs::saveAs,
        CommandIDs::showPluginListEditor,
        CommandIDs::showAppSettings,
        CommandIDs::showAudioSettings,
        CommandIDs::aboutBox,
        CommandIDs::allWindowsForward,
        CommandIDs::playPause,
        CommandIDs::copySelection,
        CommandIDs::cutSelection,
        CommandIDs::pasteSelection,
        CommandIDs::toggleMappingMode
    };

    commands.addArray (ids, numElementsInArray (ids));
}


PopupMenu MainContentComponent::getMenuForIndex (int /*topLevelMenuIndex*/, const String& menuName)
{
    PopupMenu menu;
    auto commandManager = &getCommandManager();

    if (menuName == "File")
    {
        // "File" menu
        menu.addCommandItem (commandManager, CommandIDs::newFile);
        menu.addCommandItem (commandManager, CommandIDs::open);
        menu.addCommandItem (commandManager, CommandIDs::openLastDocument);

        RecentlyOpenedFilesList recentFiles (getEngine()->getLastOpenedFileList());

        PopupMenu recentFilesMenu;
        recentFiles.createPopupMenuItems (recentFilesMenu, CommandIDs::lastFileStartID, true, true);
        menu.addSubMenu ("Open Recent", recentFilesMenu);

        menu.addCommandItem (commandManager, CommandIDs::save);
        menu.addCommandItem (commandManager, CommandIDs::saveAs);
        menu.addSeparator();
        menu.addCommandItem (commandManager, CommandIDs::showAppSettings);
        menu.addCommandItem (commandManager, CommandIDs::showAudioSettings);
        menu.addCommandItem (commandManager, CommandIDs::showPluginListEditor);
        menu.addCommandItem (commandManager, CommandIDs::aboutBox);
        menu.addSeparator();
        menu.addCommandItem (commandManager, StandardApplicationCommandIDs::quit);

    }
    else if (menuName == "Edit")
    {
        menu.addCommandItem (commandManager, CommandIDs::playPause);
        menu.addSeparator();
        menu.addCommandItem (commandManager, CommandIDs::copySelection);
        menu.addCommandItem (commandManager, CommandIDs::cutSelection);
        menu.addCommandItem (commandManager, CommandIDs::pasteSelection);

    }

    else if (menuName == "Options")
    {
        // "Options" menu
        menu.addCommandItem (commandManager, CommandIDs::toggleMappingMode);
        menu.addSeparator();

    }
    else if (menuName == "Windows")
    {
        menu = ShapeShifterManager::getInstance()->getPanelsMenu();
        menu.addSeparator();
        menu.addCommandItem (commandManager, CommandIDs::allWindowsForward);

    }


    return menu;
}

bool MainContentComponent::perform (const InvocationInfo& info)
{

    switch (info.commandID)
    {


        case CommandIDs::newFile:
        {
            int result = AlertWindow::showYesNoCancelBox (AlertWindow::QuestionIcon, juce::translate("Save document"), juce::translate("Do you want to save the document before creating a new one ?"));

            if (result != 0)
            {
                if (result == 1) engine->save (true, true);

                engine->createNewGraph();

            }
        }
        break;

        case CommandIDs::open:
        {
            int result = AlertWindow::showYesNoCancelBox (AlertWindow::QuestionIcon, juce::translate("Save document"), juce::translate("Do you want to save the document before opening a new one ?"));

            if (result != 0)
            {
                if (result == 1) engine->save (true, true);

                engine->loadFromUserSpecifiedFile (true);
            }
        }
        break;

        case CommandIDs::openLastDocument:
        {
            // TODO implement the JUCE version calling change every time something is made (maybe todo with undomanager)
            //            int result = engine->saveIfNeededAndUserAgrees();
            int result = AlertWindow::showYesNoCancelBox (AlertWindow::QuestionIcon, juce::translate("Save document"), "Do you want to save the document before opening the last one ?");

            if (result != 0)
            {
                if (result == 1) engine->save (true, true);

                engine->loadFrom (engine->getLastDocumentOpened(), true);
            }
        }
        break;

        case CommandIDs::save:
            engine->save (true, true);
            break;

        case CommandIDs::saveAs:
            engine->saveAs (File::nonexistent, true, true, true);
            break;





        case CommandIDs::showPluginListEditor:
            AppPropertiesUI::showAppSettings(AppPropertiesUI::PluginsPageName);
            break;

        case CommandIDs::showAudioSettings:
            AppPropertiesUI::showAppSettings(AppPropertiesUI::AudioPageName);
            break;

        case CommandIDs::showAppSettings:
            AppPropertiesUI::showAppSettings();
            break;

        case CommandIDs::aboutBox:
            //TODO about box
            LOG("about LGML");
            break;

        case CommandIDs::allWindowsForward:
        {
            Desktop& desktop = Desktop::getInstance();

            for (int i = 0; i < desktop.getNumComponents(); ++i)

                // doesn't work on windows
                if (desktop.getComponent (i)->getParentComponent() != nullptr)
                {
                    desktop.getComponent (i)->toBehind (this);
                }

            break;
        }

        case CommandIDs::toggleMappingMode:
            LGMLDragger::getInstance()->toggleMappingMode();
            break;

        case CommandIDs::playPause:
            TimeManager::getInstance()->togglePlay();
            break;

        case CommandIDs::copySelection:
        case CommandIDs::cutSelection:
        {
            InspectableComponent* ic = Inspector::getInstance()->getCurrentComponent();

            if (ic != nullptr)
            {
                ParameterContainer* cc = ic->getRelatedParameterContainer();

                if (cc != nullptr)
                {

                    var data (new DynamicObject());
                    data.getDynamicObject()->setProperty ("type", ic->inspectableType);
                    data.getDynamicObject()->setProperty ("data", cc->getObject());
                    if(auto relatedComponent =Inspector::getInstance()->getCurrentComponent()){
                    
                    NodeContainerViewer *  ncv = dynamic_cast<NodeContainerViewer*>(relatedComponent);
                    if(!ncv)ncv=relatedComponent->findParentComponentOfClass<NodeContainerViewer>();
                    if(ncv && ncv->uiParams){
                        auto nodeUIParams = ncv->uiParams->getControllableContainerByName(cc->shortName);
                        data.getDynamicObject()->setProperty ("uiData",nodeUIParams->getObject());
                    }

                    if (info.commandID == CommandIDs::cutSelection)
                    {
                        if (ic->inspectableType == "node") ((ConnectableNode*)cc)->remove();
                        else if (ic->inspectableType == "controller") ((Controller*)cc)->remove();

                        //            else if (ic->inspectableType == "fastMap") ((FastMap *)cc)->remove();
                    }

                    SystemClipboard::copyTextToClipboard (JSON::toString (data));
                    }
                }
            }
        }
        break;

        case CommandIDs::pasteSelection:
        {
            String clipboard = SystemClipboard::getTextFromClipboard();

            var data = JSON::parse (clipboard);

            if (data != var::null)
            {
                DynamicObject* d = data.getDynamicObject();

                if (d != nullptr && d->hasProperty ("type"))
                {

                    String type = d->getProperty ("type");
                    auto relatedComponent =Inspector::getInstance()->getCurrentComponent();

                    if (relatedComponent != nullptr)
                    {
                        if (type == "node" && relatedComponent->inspectableType == "node")
                        {
                            ConnectableNode* cn = dynamic_cast<ConnectableNode*> (relatedComponent->getRelatedParameterContainer());
                            NodeContainer* container = (dynamic_cast<NodeContainer*> (cn)) ? dynamic_cast<NodeContainer*> (cn) : cn->getParentNodeContainer();
                            
                            if (cn != nullptr)
                            {
                                ConnectableNode* n = container->addNodeFromJSONData (d->getProperty ("data").getDynamicObject());

                                // ensure to have different uuid than the one from JSON
                                if (n)
                                {
                                    n->uid = Uuid();
                                    NodeContainerViewer *  ncv = dynamic_cast<NodeContainerViewer*>(relatedComponent);
                                    if(!ncv)ncv=Inspector::getInstance()->getCurrentComponent()->findParentComponentOfClass<NodeContainerViewer>();
                                    if(ncv){
                                    auto nodeUI = ncv->getUIForNode(n);
                                    if(nodeUI){
                                        if(auto o = d->getProperty ("uiData").getDynamicObject()){
                                            auto nodeUIParams = dynamic_cast<ParameterContainer*>(ncv->uiParams->getControllableContainerByName(n->shortName));
                                            nodeUIParams->configureFromObject(o);
                                        }
                                        nodeUI->uid=Uuid();
                                        nodeUI->nodePosition->setPoint (ncv->getMouseXYRelative());
                                        nodeUI->nodeMinimizedPosition->setPoint (ncv->getMouseXYRelative());
                                    }
                                    else{
                                        jassertfalse;
                                    }
                                    }
                                    else{
                                        jassertfalse;
                                    }
                                }
                            }
                        }
                    }
                }
            }
        }
        break;

        default:
            DBG ("no command found");
            return false;
    }

    return true;
}

void MainContentComponent::menuItemSelected (int menuItemID, int topLevelMenuIndex)
{

    String menuName = getMenuBarNames()[topLevelMenuIndex ];

    if (menuName == "Windows")
    {
        ShapeShifterManager::getInstance()->handleMenuPanelCommand (menuItemID);
    }
    else if (isPositiveAndBelow (menuItemID - CommandIDs::lastFileStartID, 100))
    {
        RecentlyOpenedFilesList recentFiles (getEngine()->getLastOpenedFileList());
        engine->loadFrom (recentFiles.getFile (menuItemID - CommandIDs::lastFileStartID), true);
    }
}


StringArray MainContentComponent::getMenuBarNames()
{
    const char* const names[] = { "File", "Edit", "Options", "Windows", nullptr };
    static  StringArray namesArray (names);
    return namesArray;
}


