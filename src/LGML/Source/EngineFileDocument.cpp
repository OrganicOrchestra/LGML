/*
 ==============================================================================

 MaincComponentFileDocument.cpp
 Created: 25 Mar 2016 7:07:20pm
 Author:  Martin Hermant

 ==============================================================================
 */

#include "Engine.h"

#include "NodeFactory.h"

#include "MainComponent.h"

/*================================
 this file implements all methods that are related to saving/loading : basicly iherited from FileBasedDocument
 */


String Engine::getDocumentTitle() {
    if (! getFile().exists())
        return "Unnamed";

    return getFile().getFileNameWithoutExtension();
}


Result Engine::loadDocument (const File& file){

    if (!file.exists()) return Result::fail("File does not exist");

    ScopedPointer<InputStream> is( file.createInputStream());
    var data = JSON::parse(*is);
    loadJSONData(data);
    setLastDocumentOpened(file);
    return Result::ok();
}


Result Engine::saveDocument (const File& file){

    var data = getJSONData();

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
                                   ->getValue ("recentNodeGraphFiles"));

    return recentFiles.getFile (0);
}




void Engine::setLastDocumentOpened (const File& file) {

    RecentlyOpenedFilesList recentFiles;
    recentFiles.restoreFromString (getAppProperties().getUserSettings()
                                   ->getValue ("recentNodeGraphFiles"));

    recentFiles.addFile (file);

    getAppProperties().getUserSettings()->setValue ("recentNodeGraphFiles", recentFiles.toString());

}

var Engine::getJSONData()
{

    var data(new DynamicObject());
    var metaData(new DynamicObject());

    metaData.getDynamicObject()->setProperty("LGMLVersion",ProjectInfo::versionString);
	data.getDynamicObject()->setProperty("metaData", metaData);

	data.getDynamicObject()->setProperty("presetManager", PresetManager::getInstance()->getJSONData());
	data.getDynamicObject()->setProperty("nodeManager", NodeManager::getInstance()->getJSONData());
    data.getDynamicObject()->setProperty("controllerManager",ControllerManager::getInstance()->getJSONData());
	data.getDynamicObject()->setProperty("ruleManager", RuleManager::getInstance()->getJSONData());
	data.getDynamicObject()->setProperty("fastMapper", FastMapper::getInstance()->getJSONData());
	
    return data;
}

/// ===================
// loading

void Engine::loadJSONData (var data)
{
    //    TODO check version Compat
	clear();

	MainContentComponent::inspector->setEnabled(false); //avoid creation of inspector editor while recreating all nodes, controllers, rules,etc. from file

	DynamicObject * d = data.getDynamicObject();

	if (d->hasProperty("presetManager")) PresetManager::getInstance()->loadJSONData(d->getProperty("presetManager"));
	if (d->hasProperty("nodeManager")) NodeManager::getInstance()->loadJSONData(d->getProperty("nodeManager"));
	if (d->hasProperty("controllerManager")) ControllerManager::getInstance()->loadJSONData(d->getProperty("controllerManager"));
	if (d->hasProperty("ruleManager"))RuleManager::getInstance()->loadJSONData(d->getProperty("ruleManager"));
	if(d->hasProperty("fastMapper")) FastMapper::getInstance()->loadJSONData(d->getProperty("fastMapper"));

	MainContentComponent::inspector->setEnabled(true); //Re enable editor

}


//#if JUCE_MODAL_LOOPS_PERMITTED
//File Engine::getSuggestedSaveAsFile (const File& defaultFile){
//
//}
//#endif
