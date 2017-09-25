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


#include "Engine.h"

#include "UI/Inspector/Inspector.h"

#include "Node/Impl/AudioDeviceInNode.h"
#include "Node/Impl/AudioDeviceOutNode.h"

#include "JuceHeader.h" // for project info

/*================================
 this file implements all methods that are related to saving/loading : basicly iherited from FileBasedDocument
 */

ApplicationProperties & getAppProperties();

AudioDeviceManager & getAudioDeviceManager();

String Engine::getDocumentTitle() {
  if (! getFile().exists())
    return "Unnamed";

  return getFile().getFileNameWithoutExtension();
}

void Engine::createNewGraph(){
  loadingStartTime =  Time::currentTimeMillis();
  engineListeners.call(&EngineListener::startLoadFile);

  suspendAudio(true);
  clear();
  isLoadingFile = true;

  ConnectableNode * node = NodeManager::getInstance()->mainContainer->addNode(NodeFactory::createFromTypeID(AudioDeviceInNode::getClassId()));
  node->nodePosition->setPoint(150,100);
  node = NodeManager::getInstance()->mainContainer->addNode(NodeFactory::createFromTypeID( AudioDeviceOutNode::getClassId()));
  node->nodePosition->setPoint(450,100);
  setFile(File());
  isLoadingFile = false;
  handleAsyncUpdate();

}

Result Engine::loadDocument (const File& file){
  if(isLoadingFile){
    // TODO handle quick reloading of file
    return Result::fail("engine already loading");
  }
  isLoadingFile = true;
  engineListeners.call(&EngineListener::startLoadFile);

  if(Inspector::getInstanceWithoutCreating() != nullptr) Inspector::getInstance()->setEnabled(false); //avoid creation of inspector editor while recreating all nodes, controllers, rules,etc. from file

#ifdef MULTITHREADED_LOADING
  // force clear on main thread, safer for ui related stuffs
  clear();
  fileLoader = new FileLoader(this,file);
  fileLoader->startThread(10);
#else
  loadDocumentAsync(file);
	 triggerAsyncUpdate();
#endif


  return Result::ok();
}

//Called from fileLoader
void Engine::loadDocumentAsync(const File & file){

  suspendAudio(true);
  clearTasks();
  taskName = "Loading File";
  ProgressTask * clearTask = addTask("clearing");
  ProgressTask * parseTask = addTask("parsing");
  ProgressTask * loadTask = addTask("loading");
  clearTask->start();
  clear();
  clearTask->end();

  //  {
  //    MessageManagerLock ml;
  //  }
  ScopedPointer<InputStream> is( file.createInputStream());



  loadingStartTime =  Time::currentTimeMillis();
  setFile(file);

  // relative path can be resolved this way
  // but better to use getNormalizedFilePath(file) and getFileAtNormalizedPath(path) to handle both absolute and relative

  file.getParentDirectory().setAsCurrentWorkingDirectory();

  {
    parseTask->start();
    jsonData = JSON::parse(*is);
    parseTask->end();
    loadTask->start();
    loadJSONData(jsonData,loadTask);
    loadTask->end();


  }// deletes data before launching audio, (data not needed after loaded)

  jsonData = var();
}

void Engine::managerEndedLoading(){
  if(allLoadingThreadsAreEnded()){
    triggerAsyncUpdate();
  }
}
void Engine::managerProgressedLoading(float _progress)
{
  engineListeners.call(&EngineListener::fileProgress, _progress,0);
}
bool Engine::allLoadingThreadsAreEnded(){
  return NodeManager::getInstance()->getNumJobs()== 0 && (fileLoader && fileLoader->isEnded);
}

void Engine::fileLoaderEnded(){
  if(allLoadingThreadsAreEnded()){
    triggerAsyncUpdate();
  }
}


void Engine::handleAsyncUpdate(){


  isLoadingFile = false;
  if(getFile().exists()){
    setLastDocumentOpened(getFile());
  }

  //  graphPlayer.setProcessor(NodeManager::getInstance()->mainContainer->getAudioGraph());
  //  suspendAudio(false);
  int64 timeForLoading  =  Time::currentTimeMillis()-loadingStartTime;
  suspendAudio(false);
  engineListeners.call(&EngineListener::endLoadFile);
  NLOG("Engine","Session loaded in " << timeForLoading/1000.0 << "s");
}

Result Engine::saveDocument (const File& file){

  var data = getObject();

  if (file.exists()) file.deleteFile();
  ScopedPointer<OutputStream> os( file.createOutputStream());
  JSON::writeToStream(*os, data);
  os->flush();

  setLastDocumentOpened(file);
  return Result::ok();
}



File Engine::getLastDocumentOpened() {
  RecentlyOpenedFilesList recentFiles;
  recentFiles.restoreFromString (getAppProperties().getUserSettings()
                                 ->getValue (lastFileListKey));

  return recentFiles.getFile (0);
}




void Engine::setLastDocumentOpened (const File& file) {

  RecentlyOpenedFilesList recentFiles;
  recentFiles.restoreFromString (getAppProperties().getUserSettings()
                                 ->getValue (lastFileListKey));

  recentFiles.addFile (file);

  getAppProperties().getUserSettings()->setValue (lastFileListKey, recentFiles.toString());
  RecentlyOpenedFilesList::registerRecentFileNatively(file);

}

DynamicObject* Engine::getObject()
{

  auto data = new DynamicObject();
  var metaData(new DynamicObject());

  metaData.getDynamicObject()->setProperty("version",ProjectInfo::versionString);
  metaData.getDynamicObject()->setProperty("versionNumber", ProjectInfo::versionNumber);

  data->setProperty("metaData", metaData);

  data->setProperty("presetManager", PresetManager::getInstance()->getObject());
  data->setProperty("nodeManager", NodeManager::getInstance()->getObject());
  data->setProperty("controllerManager",ControllerManager::getInstance()->getObject());

  data->setProperty("fastMapper", FastMapper::getInstance()->getObject());

  return data;
}

/// ===================
// loading

void Engine::loadJSONData (const var & data,ProgressTask * loadingTask)
{

  DynamicObject * md = data.getDynamicObject()->getProperty("metaData").getDynamicObject();
  bool versionChecked = checkFileVersion(md);


  if (!versionChecked)
  {
    String versionString = md->hasProperty("version") ? md->getProperty("version").toString() : "?";
    AlertWindow::showMessageBox(AlertWindow::AlertIconType::WarningIcon, "You're old, bitch !", "File version (" + versionString + ") is not supported anymore.\n(Minimum supported version : " + getMinimumRequiredFileVersion() + ")");
    return;
  }


  clear();


  DynamicObject * d = data.getDynamicObject();
  ProgressTask * presetTask = loadingTask->addTask("presetManager");
  ProgressTask * nodeManagerTask = loadingTask->addTask("nodeManager");
  ProgressTask * controllerManagerTask = loadingTask->addTask("controllerManager");
  ProgressTask * fastMapperTask = loadingTask->addTask("fastMapper");

  presetTask->start();
  if (d->hasProperty("presetManager")) PresetManager::getInstance()->configureFromObject(d->getProperty("presetManager").getDynamicObject());
  presetTask->end();
  nodeManagerTask->start();
  if (d->hasProperty("nodeManager")) NodeManager::getInstance()->configureFromObject(d->getProperty("nodeManager").getDynamicObject());
  nodeManagerTask->end();
  controllerManagerTask->start();
  if (d->hasProperty("controllerManager")) ControllerManager::getInstance()->configureFromObject(d->getProperty("controllerManager").getDynamicObject());
  controllerManagerTask->end();
  fastMapperTask->start();
  if(d->hasProperty("fastMapper")) FastMapper::getInstance()->configureFromObject(d->getProperty("fastMapper").getDynamicObject());
  fastMapperTask->end();

  //Clean unused presets
  PresetManager::getInstance()->deleteAllUnusedPresets(this);

  if (Inspector::getInstanceWithoutCreating() != nullptr) Inspector::getInstance()->setEnabled(true); //Re enable editor

}

bool Engine::checkFileVersion(DynamicObject * metaData)
{
  if (!metaData->hasProperty("version")) return false;
  DBG(metaData->getProperty("version").toString() << "/ " << getMinimumRequiredFileVersion());

  StringArray fileVersionSplit;
  fileVersionSplit.addTokens(metaData->getProperty("version").toString(), juce::StringRef("."), juce::StringRef("\""));

  StringArray minVersionSplit;
  minVersionSplit.addTokens(getMinimumRequiredFileVersion(), juce::StringRef("."), juce::StringRef("\""));

  int maxVersionNumbers = jmax<int>(fileVersionSplit.size(), minVersionSplit.size());
  while (fileVersionSplit.size() < maxVersionNumbers) fileVersionSplit.add("0");
  while (minVersionSplit.size() < maxVersionNumbers) minVersionSplit.add("0");

  for (int i = 0;i < maxVersionNumbers; i++)
  {
    int fV = fileVersionSplit[i].getIntValue();
    int minV = minVersionSplit[i].getIntValue();
    if (fV > minV) return true;
    else if (fV < minV) return false;
  }

  return true;
}

String Engine::getMinimumRequiredFileVersion()
{
  return "0.2.0";
}

File Engine::getCurrentProjectFolder(){
  if(!getFile().exists()){
#if !LGML_UNIT_TESTS
    LOG("!! current session not saved, script will have an absolute path");
//    jassertfalse;
#endif
    return File();
  }
  return getFile().getParentDirectory();

}

String Engine::getNormalizedFilePath(const File & f){
  File pf(getCurrentProjectFolder());
  if(f.isAChildOf(pf)){
    return f.getRelativePathFrom(pf);

  }
  return f.getFullPathName();
}

File Engine::getFileAtNormalizedPath(const String & path){
  bool isRelative = path.length()>0 && (path[0] != (File::separator) || path[0]=='.');
  if(isRelative){
    return getCurrentProjectFolder().getChildFile(path);
  }
  else{
    return File(path);
  }
}

//#if JUCE_MODAL_LOOPS_PERMITTED
//File Engine::getSuggestedSaveAsFile (const File& defaultFile){
//
//}
//#endif
