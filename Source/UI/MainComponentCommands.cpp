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

#if !ENGINE_HEADLESS

#include "MainComponent.h"
#include "../FastMapper/FastMap.h"
#include "../Engine.h"
#include "Inspector/Inspector.h"
#include "../Node/NodeContainer/NodeContainer.h"
#include "LGMLDragger.h"
#include "AppPropertiesUI.h"

#include "../Node/NodeContainer/UI/NodeContainerViewer.h"// for copy paste
#include "../Node/Manager/UI/NodeManagerUI.h" // for copy paste

extern UndoManager & getAppUndoManager();

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
    static const int undo                   = 0x30023;
    static const int redo                   = 0x30024;
    static const int find                   = 0x30025;
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
            result.setInfo (juce::translate("New"), juce::translate("Creates a new filter graph file"), category, 0);
            result.defaultKeypresses.add (KeyPress ('n', ModifierKeys::commandModifier, 0));
            break;

        case CommandIDs::open:
            result.setInfo (juce::translate("Open..."), juce::translate("Opens a filter graph file"), category, 0);
            result.defaultKeypresses.add (KeyPress ('o', ModifierKeys::commandModifier, 0));
            break;

        case CommandIDs::openLastDocument:
            result.setInfo (juce::translate("Open Last Document"), juce::translate("Opens a filter graph file"), category, 0);
            result.defaultKeypresses.add (KeyPress ('o', ModifierKeys::shiftModifier | ModifierKeys::commandModifier, 0));
            break;

        case CommandIDs::save:
            result.setInfo (juce::translate("Save"), juce::translate("Saves the current graph to a file"), category, 0);
            result.defaultKeypresses.add (KeyPress ('s', ModifierKeys::commandModifier, 0));
            break;

        case CommandIDs::saveAs:
            result.setInfo (juce::translate("Save As..."),
                            juce::translate("Saves a copy of the current graph to a file"),
                            category, 0);
            result.defaultKeypresses.add (KeyPress ('s', ModifierKeys::shiftModifier | ModifierKeys::commandModifier, 0));
            break;

        case CommandIDs::showPluginListEditor:
            result.setInfo (juce::translate("Plug-Ins Settings..."),
                            "", category, 0);
            result.addDefaultKeypress ('p', ModifierKeys::commandModifier);
            break;

        case CommandIDs::showAudioSettings:
            result.setInfo (juce::translate("Audio settings..."), "", category, 0);
            result.addDefaultKeypress ('a', ModifierKeys::commandModifier | ModifierKeys::shiftModifier );
            break;

        case CommandIDs::showAppSettings:
            result.setInfo (juce::translate("Settings..."), "", category, 0);
            result.addDefaultKeypress (',', ModifierKeys::commandModifier);
            break;


        case CommandIDs::aboutBox:
            result.setInfo (juce::translate("About..."), "", category, 0);
            break;

        case CommandIDs::allWindowsForward:
            result.setInfo (juce::translate("All Windows Forward"), juce::translate("Bring all plug-in windows forward"), category, 0);
            result.addDefaultKeypress ('w', ModifierKeys::commandModifier);
            break;

        case CommandIDs::playPause:
            result.setInfo (juce::translate("Play/pause"), juce::translate("Play or pause LGML"), category, 0);
            result.addDefaultKeypress (' ', ModifierKeys::noModifiers);
            break;

        case CommandIDs::toggleMappingMode:
            result.setInfo (juce::translate("toggle mappingMode"), juce::translate("toggle param mapping mode"), category, 0);
            result.addDefaultKeypress ('m', ModifierKeys::commandModifier);
            break;

        case CommandIDs::copySelection:
            result.setInfo (juce::translate("Copy selection"), juce::translate("Copy current selection"), category, 0);
            result.addDefaultKeypress ('c', ModifierKeys::commandModifier);
            break;

        case CommandIDs::cutSelection:
            result.setInfo (juce::translate("Cut selection"), juce::translate("Cut current selection"), category, 0);
            result.addDefaultKeypress ('x', ModifierKeys::commandModifier);
            break;

        case CommandIDs::pasteSelection:
            result.setInfo (juce::translate("Paste selection"), juce::translate("Paste previously copied item into selected object if possible"), category, 0);
            result.addDefaultKeypress ('v', ModifierKeys::commandModifier);
            break;

        case CommandIDs::undo:
            result.setInfo (juce::translate("Undo"), juce::translate("Undo last action"), category, 0);
            result.addDefaultKeypress ('z', ModifierKeys::commandModifier);
            result.setActive(getAppUndoManager().canUndo());
            break;

        case CommandIDs::redo:
            result.setInfo (juce::translate("Redo"), juce::translate("Redo last undo"), category, 0);
            result.addDefaultKeypress ('z', ModifierKeys::commandModifier | ModifierKeys::shiftModifier);
            result.setActive(getAppUndoManager().canRedo());
            break;
        case CommandIDs::find:
            result.setInfo (juce::translate("Find"), juce::translate("Find element"), category, 0);
            result.addDefaultKeypress ('f', ModifierKeys::commandModifier );
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
        CommandIDs::undo,
        CommandIDs::redo,
        CommandIDs::find,
        CommandIDs::toggleMappingMode
    };

    commands.addArray (ids, numElementsInArray (ids));
}


PopupMenu MainContentComponent::getMenuForIndex (int /*topLevelMenuIndex*/, const String& menuName)
{
    PopupMenu menu;
    auto commandManager = &getCommandManager();

    if (menuName == juce::translate("File"))
    {
        // "File" menu
        menu.addCommandItem (commandManager, CommandIDs::newFile);
        menu.addCommandItem (commandManager, CommandIDs::open);
        menu.addCommandItem (commandManager, CommandIDs::openLastDocument);

        RecentlyOpenedFilesList recentFiles (getEngine()->getLastOpenedFileList());

        PopupMenu recentFilesMenu;
        recentFiles.createPopupMenuItems (recentFilesMenu, CommandIDs::lastFileStartID, true, true);
        menu.addSubMenu (juce::translate("Open Recent"), recentFilesMenu);

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
    else if (menuName == juce::translate("Edit"))
    {
        menu.addCommandItem(commandManager,CommandIDs::find);
        menu.addSeparator();
        menu.addCommandItem (commandManager, CommandIDs::undo);
        menu.addCommandItem (commandManager, CommandIDs::redo);
        menu.addSeparator();
        menu.addCommandItem (commandManager, CommandIDs::playPause);
        menu.addSeparator();
        menu.addCommandItem (commandManager, CommandIDs::copySelection);
        menu.addCommandItem (commandManager, CommandIDs::cutSelection);
        menu.addCommandItem (commandManager, CommandIDs::pasteSelection);

    }

    else if (menuName == juce::translate("Options"))
    {
        // "Options" menu
        menu.addCommandItem (commandManager, CommandIDs::toggleMappingMode);
        menu.addSeparator();

    }
    else if (menuName == juce::translate("Windows"))
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
            if(engine->saveIfNeededAndUserAgrees()!=FileBasedDocument::SaveResult::userCancelledSave){
                engine->createNewGraph();
            }
            break;

        case CommandIDs::open:
            if(engine->saveIfNeededAndUserAgrees()!=FileBasedDocument::SaveResult::userCancelledSave){
                engine->loadFromUserSpecifiedFile (true);
            }
            break;

        case CommandIDs::openLastDocument:
            if(engine->saveIfNeededAndUserAgrees()!=FileBasedDocument::SaveResult::userCancelledSave){
                engine->loadFrom (engine->getLastDocumentOpened(), true);
            }
            break;

        case CommandIDs::save:
            engine->save (true, true);
            break;

        case CommandIDs::saveAs:
            engine->saveAs (File(), true, true, true);
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
                if (desktop.getComponent (i)->getParentComponent() != nullptr){desktop.getComponent (i)->toBehind (this);}

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
            Array<WeakReference<InspectableComponent>> icl ( Inspector::getInstance()->getItemArray());
            
            auto nvl = ShapeShifterManager::getInstance()->getAllSPanelsOfType<NodeManagerUIViewport>(true);

            if(icl.size()>0){

                var datal = Array<var>();

                Point<int> minSelectionPoint(10e5,10e5);
                for(auto & ic:icl){
                    if(ic){
                        minSelectionPoint.x = jmin(ic->getX(),minSelectionPoint.x);
                        minSelectionPoint.y = jmin(ic->getY(),minSelectionPoint.y);
                    }
                }
                
                for(auto  ic : icl){
                    if(!ic.get())continue;
                    ParameterContainer* cc = ic->getRelatedParameterContainer();

                    if (cc != nullptr)
                    {

                        var data (new DynamicObject());
                        data.getDynamicObject()->setProperty ("type", ic->inspectableType);
                        data.getDynamicObject()->setProperty ("data", cc->createObject());
                        auto *relatedComponent = ic.get();
//                        if(auto relatedComponent =Inspector::getInstance()->getCurrentComponent()){

                            NodeContainerViewer *  ncv = dynamic_cast<NodeContainerViewer*>(relatedComponent);
                            if(!ncv)ncv=relatedComponent->findParentComponentOfClass<NodeContainerViewer>();
                            if(ncv && ncv->uiParams){
                                auto nodeUIParams = ncv->uiParams->getControllableContainerByName(cc->shortName);
                                data.getDynamicObject()->setProperty ("uiData",nodeUIParams->createObject());

                            }

                            if (info.commandID == CommandIDs::cutSelection)
                            {
                                if (ncv && ic->inspectableType == "node") ncv->removeNodeUndoable(dynamic_cast<NodeBase*>(cc));
                                else if (ic->inspectableType == "controller") ((Controller*)cc)->remove();

                                //            else if (ic->inspectableType == "fastMap") ((FastMap *)cc)->remove();
                            }
                            datal.append(data);

//                        }
                    }
                }
                var jsonVar(new DynamicObject());
                DynamicObject::Ptr  jsonObj = jsonVar.getDynamicObject();

                jsonObj->setProperty("list", datal);

                jsonObj->setProperty("minSelectionPoint", Array<var>({minSelectionPoint.x,minSelectionPoint.y}));


                SystemClipboard::copyTextToClipboard (JSON::toString (jsonVar));

            }
        }
            break;
        case CommandIDs::undo:
        {
            getAppUndoManager().undo();
        }
            break;
        case CommandIDs::redo:
        {
            getAppUndoManager().redo();
        }
            break;

        case CommandIDs::find:
        {
            auto sm =ShapeShifterManager::getInstance();
            static String pn ("Outliner");
            sm->showContent(pn);
            auto outlinerP = sm->getPanelForContentName(pn);
            if(outlinerP){
                auto content = outlinerP->getContentForName(pn);
                if(content && content->contentComponent){
                    auto searchBar = dynamic_cast<TextEditor*>(content->contentComponent->findChildWithID("search"));
                    if(searchBar){
                        searchBar->grabKeyboardFocus();
                    }
                    else{
                        DBG("Outliner should contain a search bar");
                    }
                }
            }



        }
            break;
        case CommandIDs::pasteSelection:
        {
            String clipboard = SystemClipboard::getTextFromClipboard();
            var clipboardOb =JSON::parse (clipboard);
            var datal = clipboardOb.getProperty("list", "");

            if(datal.isArray()){
                auto arr = datal.getArray();
                for(auto data:*arr){

                    DynamicObject* d = data.getDynamicObject();

                    if (d != nullptr && d->hasProperty ("type"))
                    {

                        String type = d->getProperty ("type");
                        auto relatedComponent =Inspector::getInstance()->getFirstCurrentComponent();

                        if (relatedComponent != nullptr)
                        {
                            if (type == "node" && relatedComponent->inspectableType == "node")
                            {
                                ConnectableNode* cn = dynamic_cast<ConnectableNode*> (relatedComponent->getRelatedParameterContainer());
//                                NodeContainer* container = (dynamic_cast<NodeContainer*> (cn)) ? dynamic_cast<NodeContainer*> (cn) : cn->getParentNodeContainer();

                                if (cn != nullptr)
                                {
                                    NodeBase* n = NodeFactory::createBaseFromObject ("", d->getProperty ("data").getDynamicObject());


                                    // ensure to have different uuid than the one from JSON
                                    if (n)
                                    {
                                        n->uid = Uuid();
                                        NodeContainerViewer *  ncv = dynamic_cast<NodeContainerViewer*>(relatedComponent);
                                        if(!ncv)ncv=relatedComponent->findParentComponentOfClass<NodeContainerViewer>();
                                        if(ncv){
                                            ncv->addNodeUndoable(n, Point<int>());
                                            auto nodeUI = ncv->getUIForNode(n);
                                            if(nodeUI){


                                                if(auto o = d->getProperty ("uiData").getDynamicObject()){
                                                    auto nodeUIParams = dynamic_cast<ParameterContainer*>(ncv->uiParams->getControllableContainerByName(n->shortName));
                                                    nodeUIParams->configureFromObject(o);
                                                }
                                                nodeUI->uid=Uuid();
                                                Point<int> offset(0,0);
                                                if(auto o = clipboardOb.getProperty("minSelectionPoint",var()).getArray()){
                                                    if(o->size()==2){
                                                        offset.x = o->getUnchecked(0);
                                                        offset.y = o->getUnchecked(1);
                                                        offset=ncv->getMouseXYRelative()-offset;
                                                    }
                                                }
                                                nodeUI->nodePosition->setPoint (nodeUI->nodePosition->getPoint()+offset);
                                                nodeUI->nodeMinimizedPosition->setPoint (nodeUI->nodeMinimizedPosition->getPoint()+offset);
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
    
    if (menuName == juce::translate("Windows"))
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
    
    StringArray namesArray (
                            juce::translate("File"),
                            juce::translate("Edit"),
                            juce::translate("Options"),
                            juce::translate("Windows") );
    return namesArray;
}


#endif
