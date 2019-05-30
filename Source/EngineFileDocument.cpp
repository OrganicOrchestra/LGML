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



#include "Node/Impl/AudioDeviceInNode.h"
#include "Node/Impl/AudioDeviceOutNode.h"
#include "Controller/Impl/OSCJsController.h"


#include "Engine.h"

#if !ENGINE_HEADLESS
#include "UI/Inspector/Inspector.h"
#include "Node/Manager/UI/NodeManagerUI.h"
#endif

/*================================
 this file implements all methods that are related to saving/loading : basicly iherited from FileBasedDocument
 */

ApplicationProperties* getAppProperties();

AudioDeviceManager& getAudioDeviceManager();
String lastFileListKey ("lastFileList");

String Engine::getDocumentTitle()
{
    if (! getFile().exists())
        return "Unnamed";

    return getFile().getFileNameWithoutExtension();
}

RecentlyOpenedFilesList Engine::getLastOpenedFileList(){
    RecentlyOpenedFilesList recentFiles;
    recentFiles.restoreFromString (getAppProperties()->getUserSettings()
                                   ->getValue (lastFileListKey));
    return recentFiles;
}

void Engine::createNewGraph()
{
    loadingStartTime =  getElapsedMillis();
    engineListeners.call (&EngineListener::startLoadFile);

    suspendAudio (true);
    clear();
    isLoadingFile = true;

    NodeManager::getInstance()->addNode (NodeFactory::createFromTypeID (AudioDeviceInNode::typeId()));


    NodeManager::getInstance()->addNode (NodeFactory::createFromTypeID ( AudioDeviceOutNode::typeId()));

    setFile (File());
    isLoadingFile = false;

    handleAsyncUpdate();

}

Result Engine::loadDocument (const File& file)
{
    if (isLoadingFile)
    {
        return Result::fail ("engine already loading");
    }

    isLoadingFile = true;
    engineListeners.call (&EngineListener::startLoadFile);
#if !ENGINE_HEADLESS
    if (Inspector::getInstanceWithoutCreating() != nullptr) Inspector::getInstance()->shouldListen (false); //avoid creation of inspector editor while recreating all nodes, controllers, rules,etc. from file
#endif

#ifdef MULTITHREADED_LOADING
    // force clear on main thread, safer for ui related stuffs
    if(getAppProperties()->getUserSettings()->getBoolValue("multiThreadedLoading",true)){
        clear();
        fileLoader = new FileLoader (this, file);
        fileLoader->startThread (10);
    }
    else
#endif
    {
        loadDocumentAsync (file);
        triggerAsyncUpdate();
    }


    return Result::ok();
}

//Called from fileLoader
void Engine::loadDocumentAsync (const File& file)
{

    suspendAudio (true);
    clearTasks();
    taskName = juce::translate("Loading File");
    ProgressTask* clearTask = addTask (juce::translate("clearing"));
    ProgressTask* parseTask = addTask (juce::translate("parsing"));
    ProgressTask* loadTask = addTask (juce::translate("loading"));
    clearTask->start();
    clear();
    clearTask->end();

    //  {
    //    MessageManagerLock ml;
    //  }
    ScopedPointer<InputStream> is ( file.createInputStream());



    loadingStartTime =  getElapsedMillis();
    setFile (file);

    // relative path can be resolved this way
    // but better to use getNormalizedFilePath(file) and getFileAtNormalizedPath(path) to handle both absolute and relative

    file.getParentDirectory().setAsCurrentWorkingDirectory();


    parseTask->start();
    var jsonData = JSON::parse (*is);
    parseTask->end();
    loadTask->start();
    loadJSONData (jsonData, loadTask);
    loadTask->end();


    // deletes data before launching audio, (data not needed after loaded)
    jsonData = var();
}

void Engine::managerEndedLoading()
{
    if (allLoadingThreadsAreEnded())
    {
        triggerAsyncUpdate();
    }
}
void Engine::managerProgressedLoading (float _progress)
{
    engineListeners.call (&EngineListener::fileProgress, _progress, 0);
}
bool Engine::allLoadingThreadsAreEnded()
{
    return NodeManager::getInstance()->getNumJobs() == 0 && (fileLoader && fileLoader->isEnded);
}

void Engine::fileLoaderEnded()
{
    if (allLoadingThreadsAreEnded())
    {
        triggerAsyncUpdate();
    }
}


void Engine::handleAsyncUpdate()
{


    isLoadingFile = false;
    auto cfp = getCurrentProjectFolder();
    if(cfp.exists()){
        for(auto & f:fileSavers){
            f->loadFiles(cfp);
        }
    }
    //  graphPlayer.setProcessor(NodeManager::getInstance()->getAudioGraph());
    //  suspendAudio(false);
    auto timeForLoading  =  getElapsedMillis() - loadingStartTime;
    suspendAudio (false);
    if(hasDefaultOSCControl){
        auto controllers = ControllerManager::getInstance()->getContainersOfType<OSCJsController>(false);
        OSCJsController *mainC(nullptr);
        for(auto & c:controllers){
            if(c->fullSync->boolValue()){
                mainC = c;
                break;
            }
        }
        if(!mainC){
            mainC = (OSCJsController*)ControllerFactory::createFromTypeID(OSCJsController::typeId());
            mainC->fullSync->setValue(true);
            ControllerManager::getInstance()->addController(mainC);
        }
    }
    engineListeners.call (&EngineListener::endLoadFile);
    NLOG ("Engine", juce::translate("Session loaded in 123s").replace("123", String(timeForLoading / 1000.0)));
}

Result Engine::saveDocument (const File& file)
{

    var data = createObject();
    String errMsg;
    if (file.exists())
        file.deleteFile();

    {
        ScopedPointer<OutputStream> os ( file.createOutputStream());
        JSON::writeToStream (*os, data);
        os->flush();
    }
    for(auto saver : fileSavers){
        if(saver->isDirty()){
            auto r = saver->saveFiles(getCurrentProjectFolder());
            if(!r){
                errMsg+=r.getErrorMessage() + "\n";
            }
        }
    }
    setLastDocumentOpened (file);
    saveSession->setValueFrom(this, getFile().getFullPathName());
    if(errMsg.isEmpty())
        return Result::ok();
    else
        return Result::fail(errMsg);
}



File Engine::getLastDocumentOpened()
{
    RecentlyOpenedFilesList recentFiles;
    recentFiles.restoreFromString (getAppProperties()->getUserSettings()
                                   ->getValue (lastFileListKey));

    return recentFiles.getFile (0);
}




void Engine::setLastDocumentOpened (const File& file)
{

    RecentlyOpenedFilesList recentFiles;
    recentFiles.restoreFromString (getAppProperties()->getUserSettings()
                                   ->getValue (lastFileListKey));

    recentFiles.addFile (file);

    getAppProperties()->getUserSettings()->setValue (lastFileListKey, recentFiles.toString());
    RecentlyOpenedFilesList::registerRecentFileNatively (file);

}

DynamicObject* Engine::createObject()
{

    auto data = new DynamicObject();
    var metaData (new DynamicObject());

    metaData.getDynamicObject()->setProperty ("version",
                                              Engine::versionString
                                              );
    metaData.getDynamicObject()->setProperty ("versionNumber", Engine::versionNumber);

    data->setProperty ("metaData", metaData);

    //    data->setProperty ("presetManager", PresetManager::getInstance()->createObject());


    if( auto p = getControllableContainerByName("NodesUI")){
        data->setProperty("NodesUI",p->createObject());
    }

    data->setProperty ("nodeManager", NodeManager::getInstance()->createObject());
    data->setProperty ("controllerManager", ControllerManager::getInstance()->createObject());
    data->setProperty("timeManager" , TimeManager::getInstance()->createObject());
    data->setProperty ("fastMapper", FastMapper::getInstance()->createObject());

    return data;
}

/// ===================
// loading

void Engine::loadJSONData (const var& data, ProgressTask* loadingTask)
{

    DynamicObject* md = data.getDynamicObject()->getProperty ("metaData").getDynamicObject();
    bool versionChecked = checkFileVersion (md);


    if (!versionChecked)
    {
        String _versionString = md->hasProperty ("version") ? md->getProperty ("version").toString() : "?";
        if(!AlertWindow::showOkCancelBox (AlertWindow::AlertIconType::WarningIcon,
                                          juce::translate("this file was created with a diffirent LGML version \n(potentially incompatible)"),
                                          juce::translate("File version  : 123\nsupported LGML versions : 456").replace("123", _versionString).replace("456", getSupportedVersionMask().toString()),
                                          juce::translate("try anyway !"))){
            return;
        }
    }


    clear();


    DynamicObject* d = data.getDynamicObject();
    ProgressTask* presetTask = loadingTask->addTask (juce::translate("presetManager"));
    ProgressTask* timeManagerTask = loadingTask->addTask(juce::translate("timeManager"));
    ProgressTask* nodeManagerTask = loadingTask->addTask (juce::translate("nodeManager"));
    ProgressTask* controllerManagerTask = loadingTask->addTask (juce::translate("controllerManager"));
    ProgressTask* fastMapperTask = loadingTask->addTask (juce::translate("fastMapper"));

    presetTask->start();

    //    if (d->hasProperty ("presetManager")) PresetManager::getInstance()->configureFromObject (d->getProperty ("presetManager").getDynamicObject());

    presetTask->end();

    timeManagerTask->start();
    if (d->hasProperty ("timeManager")) TimeManager::getInstance()->configureFromObject(d->getProperty("timeManager").getDynamicObject());
    timeManagerTask->end();

    nodeManagerTask->start();

    if (d->hasProperty ("nodeManager")) NodeManager::getInstance()->configureFromObject (d->getProperty ("nodeManager").getDynamicObject());

#if !ENGINE_HEADLESS
    if(d->hasProperty("NodesUI")) {
        auto p = dynamic_cast<ParameterContainer*>(getControllableContainerByName("NodesUI"));

        if(!p){
            jassertfalse;
            p = new ParameterContainer("NodesUI");
            addChildControllableContainer(p);
        }
        p->configureFromObject(d->getProperty("NodesUI").getDynamicObject());

    }
#endif



    nodeManagerTask->end();
    controllerManagerTask->start();

    if (d->hasProperty ("controllerManager")) ControllerManager::getInstance()->configureFromObject (d->getProperty ("controllerManager").getDynamicObject());

    controllerManagerTask->end();
    fastMapperTask->start();

    if (d->hasProperty ("fastMapper")) FastMapper::getInstance()->configureFromObject (d->getProperty ("fastMapper").getDynamicObject());

    fastMapperTask->end();

    //Clean unused presets
    PresetManager::getInstance()->deleteAllUnusedPresets (this);
#if !ENGINE_HEADLESS
    if (auto inspector = Inspector::getInstanceWithoutCreating() ) inspector->shouldListen (true); //Re enable editor
#endif

}

bool Engine::checkFileVersion (DynamicObject* metaData)
{
    if (!metaData->hasProperty ("version")) return false;
    auto versionMask = getSupportedVersionMask();

    DBG (metaData->getProperty ("version").toString() << "/ " << versionMask.toString());

    VersionTriplet fileVersion =metaData->getProperty ("version").toString();

    return fileVersion.isCompatible(versionMask);

}

VersionTriplet Engine::getSupportedVersionMask()
{
    // minor version is marked as breaking per default
    return VersionTriplet  {String((Engine::versionNumber >> 16) & 0xFF) ,String((Engine::versionNumber >> 8 ) & 0xFF),"x" };
}

File Engine::getCurrentProjectFolder()
{
    if (!getFile().exists())
    {
#if !LGML_UNIT_TESTS
        LOGW(juce::translate("current session not saved, related files will have an absolute path"));
        //    jassertfalse;
#endif
        return File();
    }

    return getFile().getParentDirectory();

}

String Engine::getNormalizedFilePath (const File& f)
{
    File pf (getCurrentProjectFolder());

    if (f.isAChildOf (pf))
    {
        return f.getRelativePathFrom (pf);

    }

    return f.getFullPathName();
}

File Engine::getFileAtNormalizedPath (const String& path)
{
    bool isRelative = path.length() > 0 && !File::isAbsolutePath(path);
    
    if (isRelative)
    {
        return getCurrentProjectFolder().getChildFile (path);
    }
    else
    {
        return File (path);
    }
}

//#if JUCE_MODAL_LOOPS_PERMITTED
//File Engine::getSuggestedSaveAsFile (const File& defaultFile){
//
//}
//#endif
