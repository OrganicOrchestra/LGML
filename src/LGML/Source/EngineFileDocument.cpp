/*
 ==============================================================================

 MaincComponentFileDocument.cpp
 Created: 25 Mar 2016 7:07:20pm
 Author:  Martin Hermant

 ==============================================================================
 */

#include "Engine.h"

#include "NodeFactory.h"

/*================================
 this file implements all methods that are related to saving/loading : basicly iherited from FileBasedDocument
 */


String Engine::getDocumentTitle() {
    if (! getFile().exists())
        return "Unnamed";

    return getFile().getFileNameWithoutExtension();
}


Result Engine::loadDocument (const File& file){

    ScopedPointer<InputStream> is( file.createInputStream());
    var data = JSON::parse(*is);

    loadJSONData(data);
    return Result::ok();
}


Result Engine::saveDocument (const File& file){

    var data = getJSONData();

    if (file.exists()) file.deleteFile();
    ScopedPointer<OutputStream> os( file.createOutputStream());
    JSON::writeToStream(*os, data);
    os->flush();


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

    var nodeManagerData(new DynamicObject());
    var controllerManagerData(new DynamicObject());

    metaData.getDynamicObject()->setProperty("LGMLVersion",ProjectInfo::versionString);

    data.getDynamicObject()->setProperty("meta", metaData);
    data.getDynamicObject()->setProperty("nodeManager", NodeManager::getInstance()->getJSONData());
    data.getDynamicObject()->setProperty("controllerManager",ControllerManager::getInstance()->getJSONData());

    return data;
}

/// ===================
// loading

void Engine::loadJSONData (var data, bool clearManagers)
{
    clear();
    //    TODO check version Compat

    NodeManager::getInstance()->loadJSONData(data.getProperty("nodeManager", var()), clearManagers);
    ControllerManager::getInstance()->loadJSONData(data.getProperty("controllerManager", var()), clearManagers);
}


//#if JUCE_MODAL_LOOPS_PERMITTED
//File Engine::getSuggestedSaveAsFile (const File& defaultFile){
//
//}
//#endif
