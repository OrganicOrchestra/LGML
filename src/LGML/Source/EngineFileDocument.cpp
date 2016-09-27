/*
 ==============================================================================

 MaincComponentFileDocument.cpp
 Created: 25 Mar 2016 7:07:20pm
 Author:  Martin Hermant

 ==============================================================================
 */

#include "Engine.h"
#include "NodeFactory.h"
#include "Inspector.h"


/*================================
 this file implements all methods that are related to saving/loading : basicly iherited from FileBasedDocument
 */

ApplicationProperties & getAppProperties();

String Engine::getDocumentTitle() {
    if (! getFile().exists())
        return "Unnamed";

    return getFile().getFileNameWithoutExtension();
}


Result Engine::loadDocument (const File& file){





    ScopedPointer<InputStream> is( file.createInputStream());
    suspendAudio(true);
    {
        var data = JSON::parse(*is);
        loadJSONData(data);
    }// deletes data before launching audio, (data not needed after loaded)
    setLastDocumentOpened(file);
    suspendAudio(false);
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
                                   ->getValue (lastFileListKey));

    return recentFiles.getFile (0);
}




void Engine::setLastDocumentOpened (const File& file) {

    RecentlyOpenedFilesList recentFiles;
    recentFiles.restoreFromString (getAppProperties().getUserSettings()
                                   ->getValue (lastFileListKey));

    recentFiles.addFile (file);

    getAppProperties().getUserSettings()->setValue (lastFileListKey, recentFiles.toString());

}

var Engine::getJSONData()
{

    var data(new DynamicObject());
    var metaData(new DynamicObject());

    metaData.getDynamicObject()->setProperty("version",ProjectInfo::versionString);
    metaData.getDynamicObject()->setProperty("versionNumber", ProjectInfo::versionNumber);

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

    DynamicObject * md = data.getDynamicObject()->getProperty("metaData").getDynamicObject();
    bool versionChecked = checkFileVersion(md);


    if (!versionChecked)
    {
        String versionString = md->hasProperty("version") ? md->getProperty("version").toString() : "?";
        AlertWindow::showMessageBox(AlertWindow::AlertIconType::WarningIcon, "You're old, bitch !", "File version (" + versionString + ") is not supported anymore.\n(Minimum supported version : " + getMinimumRequiredFileVersion() + ")");
        return;
    }


    clear();

    if(Inspector::getInstanceWithoutCreating() != nullptr) Inspector::getInstance()->setEnabled(false); //avoid creation of inspector editor while recreating all nodes, controllers, rules,etc. from file

    DynamicObject * d = data.getDynamicObject();

    if (d->hasProperty("presetManager")) PresetManager::getInstance()->loadJSONData(d->getProperty("presetManager"));
    if (d->hasProperty("nodeManager")) NodeManager::getInstance()->loadJSONData(d->getProperty("nodeManager"));
    if (d->hasProperty("controllerManager")) ControllerManager::getInstance()->loadJSONData(d->getProperty("controllerManager"));
    if (d->hasProperty("ruleManager"))RuleManager::getInstance()->loadJSONData(d->getProperty("ruleManager"));
    if(d->hasProperty("fastMapper")) FastMapper::getInstance()->loadJSONData(d->getProperty("fastMapper"));

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
        if (fV < minV) return false;
    }

    return true;
}

String Engine::getMinimumRequiredFileVersion()
{
    return "0.2.0";
}

//#if JUCE_MODAL_LOOPS_PERMITTED
//File Engine::getSuggestedSaveAsFile (const File& defaultFile){
//
//}
//#endif
