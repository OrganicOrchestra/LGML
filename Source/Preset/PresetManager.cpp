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

#include "PresetManager.h"
#include "Preset.h"
#include "../Controllable/Parameter/ParameterContainer.h"


juce_ImplementSingleton (PresetManager)

#include "../Engine.h" // save files

static String presetFolderName("presets");
static String presetExtension(".lgml.preset");

class EngineSync : public Engine::EngineFileSaver{
public:

    EngineSync(PresetManager * _pm):EngineFileSaver("PresetManager"),pm(_pm){
        

    }

    Result saveFiles(const File & baseFolder) final{

        auto pFolder = baseFolder.getChildFile(presetFolderName);
        pFolder.deleteRecursively();

        auto r= pFolder.createDirectory();
        if(!r)return r;
        Array<Preset*> invalidPresets;
        for(auto & p:pm->presets){
            if(p && !p->isValidPreset()){
                invalidPresets.add(p);
            }
        }
        bool removeOldPresets = true;
#if !ENGINE_HEADLESS
        if(invalidPresets.size()){
        String presetsInfos;
        for(auto & p:invalidPresets){
            auto originAddress = p->getOriginContainer()?p->getOriginContainer()->getControlAddress().toString():String("Unknown");
            presetsInfos+=String("preset : @@1 was in deleted @@2\n").replace("@@1",p->getPresetName()).replace("@@2",p->originAddress->stringValue());
        }
        removeOldPresets = AlertWindow::showOkCancelBox(AlertWindow::AlertIconType::QuestionIcon,
                                                        "PresetWarning",
                                                        juce::translate("presets will be deleted :\n @@1").replace("@@1",presetsInfos),
                                                        juce::translate("Yes"), //1
                                                        juce::translate("Cancel"), // 0
                                                        nullptr,//associatedComponent
                                                        nullptr);//callback
        }
#endif
        if(removeOldPresets){
            for(auto & p:invalidPresets){
                pm->removePreset(p);
            }
        }

        for(auto &p:pm->presets){

            auto tf =pFolder.getChildFile(p->getType());
            if(!tf.exists()){
                auto rr = tf.createDirectory();
                if(!rr)return rr;
            }
            tf = tf.getChildFile(p->getPresetName()+"_"+p->getFilter()+presetExtension);
            if(tf.exists()){
                tf.deleteFile();
                jassertfalse;
            }
            tf.create();
            var data = p->createObject();
            std::unique_ptr<OutputStream> os ( tf.createOutputStream());
            JSON::writeToStream (*os, data);
            os->flush();
        }


        return Result::ok();
    }

    Result loadFiles(const File & baseFolder) final{
        pm->clear();
        String errMsg;
        auto pFolder = baseFolder.getChildFile(presetFolderName);
        if(!pFolder.exists()){
            return Result::ok();
        }

        for(auto &pf:pFolder.findChildFiles(File::TypesOfFileToFind::findFiles,true,"*"+presetExtension)){
            var pvar = JSON::parse(pf);
            DynamicObject * pobj = pvar.getDynamicObject();
            auto * pre = new Preset("unloaded Preset",nullptr);
            pre->configureFromObject(pobj);
            pm->addPreset(pre);


        }

        if(errMsg.isEmpty())
            return Result::ok();
        else
            return Result::fail(errMsg);

    }

    bool isDirty() final{
        // TODO implement preset watch
        return true;
    }

    File rootFolder;
    PresetManager * pm;
};

PresetManager::PresetManager():
engineSync (new EngineSync(this))
{


}

PresetManager::~PresetManager()
{
    clear();



}


PresetManager::Listener::Listener(){
//    PresetManager::getInstance()->presetListeners.add(this);    
}

PresetManager::Listener::~Listener(){
//    PresetManager::getInstance()->presetListeners.remove(this);
}




Preset* PresetManager::addPreset(Preset * pre){
    presets.add (pre);
    presetListeners.call(&PresetManager::Listener::presetAdded,pre);
    return pre;

}
void PresetManager::removePreset(Preset * pre){

    presets.removeObject(pre,false);
    presetListeners.call(&PresetManager::Listener::presetRemoved,pre);
    delete pre;

}

Preset* PresetManager::getPreset (const String & filter, const String& name) const
{
    for (auto& pre : presets)
    {
        if (pre->getFilter() == filter && pre->getPresetName() == name) return pre;
    }

//    DBG(String("asked : ")+filter+"::"+name);
//    for (auto& pre : presets)
//    {
//        DBG(pre->getFilter() + "::"+pre->getPresetName()+"\n");
//    }
//    jassertfalse;
    return nullptr;
}





int PresetManager::getNumPresetForFilter (const String& filter) const
{
    int num = 0;

    for (auto& pre : presets)
    {

        if (pre->getFilter() == filter)
        {
            num++;
        }
    }

    return num;
}

void PresetManager::deleteAllUnusedPresets (ParameterContainer* rootContainer)
{
    Array<Preset*> presetsToRemove;
    auto allContainers = rootContainer->getContainersOfType<ParameterContainer> (true);

    for (auto& p : presets)
    {
        bool isUsed = false;

        for (auto& cc : allContainers)
        {
            if (cc->presetable->getPresetFilter() == p->getFilter())
            {
                isUsed = true;
                break;
            }
        }

        if (!isUsed) presetsToRemove.add (p);
    }

    int numPresetsToRemove = presetsToRemove.size();

    for (auto& p : presetsToRemove){
        removePreset(p);
    }

    LOG ("Cleaned up " << numPresetsToRemove << " unused presets, now " << presets.size() << "presets");
}

int PresetManager::deletePresetsForContainer (ParameterContainer* container, bool recursive)
{
    if (container == nullptr) return 0;

    const String filter = container->presetable->getPresetFilter();

    Array<Preset*> presetsToRemove;

    for (auto& p : presets)
    {
        if (p->getFilter() == filter) presetsToRemove.add (p);
    }

    int numPresetsDeleted = presetsToRemove.size();

    for (auto& p : presetsToRemove){
        removePreset(p);
    }

    if (recursive)
    {
        for (auto& cc : container->getContainersOfType<ParameterContainer> (false))
        {
            if (cc)
            {
                numPresetsDeleted += deletePresetsForContainer (cc, true);
            }
        }
    }

    return numPresetsDeleted;
}

void PresetManager::clear()
{
    for(int i = presets.size() -1 ; i>=0 ; i--){
        removePreset(presets[i]);
    }

    
}


Array<WeakReference<Preset> >  PresetManager::getPresetsForFilter(String filter){
    Array<WeakReference<Preset> > res;
    for(auto & p:presets){
        auto fName =p->getFilter();
        if(fName==filter){
            res.add(p);
        }
    }
    return res;

}

Array<WeakReference<Preset> >  PresetManager::getPresetsForType(String type,ParameterContainer *ownerToIgnore){
    Array<WeakReference<Preset> > res;
    for(auto & p:presets){

        if(p->getType()==type){
            if(p->getOriginContainer() && p->getOriginContainer()!=ownerToIgnore)
                res.add(p);
        }
    }
    return res;

}



