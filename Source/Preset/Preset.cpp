/*
 ==============================================================================

 Preset.cpp
 Created: 8 Dec 2018 8:38:36pm
 Author:  Martin Hermant

 ==============================================================================
 */

#include "Preset.h"
#include "../Controllable/Parameter/ParameterContainer.h"
#include "PresetManager.h"

#include "../Controllable/Parameter/ParameterFactory.h"
#include "../Node/NodeContainer/NodeContainer.h"
//////////////////////////////
// Presets
String generateName(const String & name, ParameterContainer * pc){
    if(!pc){return name;}
    else{return name+pc->getSubTypeName()+pc->presetable->getPresetFilter();}
}

Preset::Preset (const String& _name, ParameterContainer * pc):
ParameterContainer(generateName(_name, pc))
{
    jassert(!_name.isEmpty());

    filter = addNewParameter<StringParameter>("filter", "filter for preset", pc?pc->presetable->getPresetFilter():"no");
    presetName = addNewParameter<StringParameter>("presetName", "name for preset", _name);
    type = addNewParameter<StringParameter>("type", "type for preset", pc?pc->getFactoryTypeId().toString():"no");
    subType = addNewParameter<StringParameter>("subType", "subType for preset", pc?pc->getSubTypeName():"no");
    originUID = addNewParameter<StringParameter>("originUID", "originUID for preset", pc?pc->uid.toString():"no");
    originAddress = addNewParameter<StringParameter>("originAddress", "originAddress of preset", pc?pc->getControlAddress().toString():"Unknown");
    
}

void Preset::clear()
{
    values = var();
}


void Preset::updateSubTypeName(){
    if(auto pc = getOriginContainer()){
        subType->setValueFrom(this,pc->getSubTypeName());
    }
    else{
        jassertfalse;
    }
}

DynamicObject * Preset::getPresetValueObject(){
    return values.getDynamicObject();
}

void Preset::configurePresetValueObject(DynamicObject *v){values = v->clone();}

DynamicObject* Preset::createObject() {
    DynamicObject * d =  Presetable::createPresetObject(this);
    if(d==nullptr){jassertfalse; LOGE("preset can not create object");return nullptr;}
    d->setProperty("values", values);
    return d;

}

bool Preset::isValidPreset(){
    return getOriginContainer()!=nullptr;
}

void Preset::configureFromObject(DynamicObject *data) {
    if(data==nullptr){jassertfalse; LOGE("preset try to configure from empty object");return;}
    ParameterContainer::configureFromObject(data);
    values = data->getProperty("values");

}
/////////////////
// Presetable


class PresetSync:public PresetManager::Listener{
    public :
    PresetSync(Presetable*p):owner(p){
        PresetManager::getInstance()->presetListeners.add(this);
    }
    ~PresetSync(){
        PresetManager::getInstance()->presetListeners.remove(this);
    }

    void presetSaved(Preset * p) final{
        if(p==owner->currentPreset){
            owner->loadPreset(p,false);
        }
    }
    void presetAdded(Preset * )final{}
    void presetRemoved(Preset * p)final{
        if(owner && p==owner->currentPreset){
            owner->loadPreset (nullptr);
        }
    }
    Presetable * owner;

};

const Identifier Presetable::presetIdentifier ("preset");

//int numPresetDBG = 0;
Presetable::Presetable(ParameterContainer  * _pc):
pc(_pc){
    
    currentPresetName = pc->addParameter(new StringParameter(presetIdentifier.toString(), "Current Preset", ""),-1,false);
    currentPresetName->addParameterListener(this);
//    currentPresetName->addAsyncCoalescedListener(this);
//    currentPresetName->queuedNotifier->debugFlag = true;
//    currentPresetName->queuedNotifier->name = currentPresetName->controlAddress.toString() + String(numPresetDBG++);
    currentPresetName->isHidenInEditor = true;
    currentPresetName->isSavable=false;
    params.add(currentPresetName);
    savePresetTrigger = pc->addParameter(new Trigger("Save Preset", "Save current preset"),-1,false);
    savePresetTrigger->addParameterListener(this);
    savePresetTrigger->isHidenInEditor = true;
    params.add(savePresetTrigger);

}
Presetable::~Presetable(){
    if(presetSync)
        delete presetSync;
}

void Presetable::containerWillClear (ControllableContainer* c)
{


    if (c == pc){
        cleanUpPresets();
    }
}
bool Presetable::isOneOfPresetableParam(ParameterBase * p){return params.contains(p);}


String getLinkInPresetName(const String & n){
    if(n.contains("(")){
        StringArray t;
        t.addTokens(n, "()", "\"");
        if(t.size()>=2){
            return t[1];
        }
    }
    return "";
}

void Presetable::newMessage (const ParameterBase::ParamWithValue&  pv)
{
    jassertfalse;
}
void Presetable::parameterValueChanged ( ParameterBase* p, ParameterBase::Listener * ){
//}

//    auto p = pv.parameter;



    if (p == savePresetTrigger)
    {
        saveCurrentPreset();

    }
    else if (p== currentPresetName)
    {
        auto * pm = PresetManager::getInstance();

        String name = p->stringValue();
        String exterUID = getLinkInPresetName(name);
        Preset* preset = nullptr;
        if(exterUID.isEmpty()){
            if(presetSync){delete presetSync;presetSync = nullptr;}
            preset = pm->getPreset (getPresetFilter(), name);
        }
        else{
            name = name.substring(0, name.indexOf("("));
            if(!presetSync){presetSync = new PresetSync(this);};


            Uuid ui(exterUID);
            if(auto ppp = ParameterContainer::getForUidGlobal(ui)){
                preset = pm->getPreset(ppp->presetable->getPresetFilter(), name);
            }
            else{
                jassertfalse;
            }

        }
       // if(currentPreset!=preset)
            loadPreset(preset);
    }

}


////////////
// Preset in controllableContainers


Preset* Presetable::addNamedPreset (const String& name,bool doLoad,void * notif)
{
    //Array<PresetValue *> vPresets;

    auto pm = PresetManager::getInstance();
    String filter = getPresetFilter();
    //String subType = pc->getSubTypeName();
    Preset* pre = pm->getPreset (filter, name);
    bool presetExists = pre != nullptr;

    if (!presetExists)
    {
        pre = new Preset (name, pc);
    }
    else
    {
        pre->clear();
    }

    var obj = createPresetObject(pc);
    String ts = JSON::toString(obj);
    pre->configurePresetValueObject(obj.getDynamicObject());
    String vTs = JSON::toString(pre->getPresetValueObject());
    if (!presetExists) {
        pm->addPreset(pre);
    }
    if(pre)pm->notifyPresetSaved((PresetManager::Listener*) notif, pre);
    if(doLoad){
        currentPreset = pre;
        currentPresetName->setValueFrom(this, pre?pre->getPresetName():"",false,true);// force
        presetableListeners.call (&Presetable::Listener::controllableContainerPresetLoaded, pc,currentPreset);
    }
    DBG(ts);



    
    
    return pre;
}


bool Presetable::loadPreset (Preset* preset,bool /*sendNotif*/)
{

    if (preset == nullptr)
    {
        currentPresetName->setValueFrom (this,"", true);
        currentPreset=nullptr;
        return false;
    }
    if(!pc){jassertfalse; LOGE("presets corrupted");return false;}
    if(auto nc = dynamic_cast<NodeContainer*>(pc)){
        // prevent assertion of resetting BuidSessionGraph to true,
        // when we are loading a preset, it's normal to call configureFromObject from within another configureFrombject
        nc->setBuildSessionGraph(false);

    }
    lastLoadPresetTime = Time::currentTimeMillis();
    DBG("loading preset" +preset->getPresetName()+ " -> " + pc->getNiceName());
    pc->configureFromObjectOrValues(preset->getPresetValueObject(),false);
    if(currentPreset){currentPreset->updateSubTypeName();};
    currentPreset = preset;
    String preName = currentPreset->getPresetName();
    if(currentPreset->getOriginContainer()!=pc){
        preName+=String("(123)").replace("123", currentPreset->getOriginUID());
    }
    auto oldV = currentPresetName->stringValue();
    currentPresetName->setValueFrom (this,preName, false,false);

    presetableListeners.call (&Presetable::Listener::controllableContainerPresetLoaded, pc,preset);
    
    return true;
}

Preset* Presetable::saveNewPreset (const String& _name,void * notif)
{
    Preset* pre = addNamedPreset(_name,true,notif);
    NLOG (pc->getControlAddress().toString(), juce::translate("New preset saved : ") + pre->getPresetName());
    //    loadPreset (pre);
    return pre;
}
void Presetable::deletePreset(Preset * pre){
    if(pre==currentPreset){
        currentPreset = nullptr;
    }
    PresetManager::getInstance()->removePreset(pre);
    
}

const Preset * Presetable::getCurrentPreset()const{
    return currentPreset;

}


DynamicObject * Presetable::createPresetObject(ParameterContainer * p){
    if(p){
        return  p->
        createObjectFiltered(
                             [p](ParameterBase * c){
                                 bool isValid = c->isPresettable;
                                 if(c->parentContainer==p){
                                     bool isRootPresetName = c==p->presetable->currentPresetName; // don't save root preset
                                     bool isRootEnable = false;
                                     if(auto n = dynamic_cast<NodeBase*>(p)){
                                         isRootEnable = c==n->enabledParam; // don't save root enabled state??
                                     }
                                     isValid &=!isRootPresetName;
//                                     isValid &=!(isRootEnable || isRootPresetName);

                                 }

                                 if(auto *pp = dynamic_cast<ParameterContainer*>(c->parentContainer.get())){
                                     if(pp->parentContainer==p ){ // sub containers at depth 1
                                         String presetN = pp->presetable->currentPresetName->stringValue();
                                         bool isPresetLoaded = !presetN.isEmpty();
                                         //bool isSamePresetLoaded = isPresetLoaded && (presetN==p->presetable->currentPresetName->stringValue());
                                         bool isCurrentPresetNameParam = c==pp->presetable->currentPresetName;
                                         bool isEnableParam = false;
                                         if(auto n = dynamic_cast<NodeBase*>(pp)){
                                             isEnableParam = c==n->enabledParam;
                                         }
                                         if(isPresetLoaded){
                                             isValid &= isCurrentPresetNameParam;
//                                             isValid&= (isCurrentPresetNameParam || isEnableParam);
                                         }
                                     }
                                 }
                                 return isValid;
                             },
                             [p](ParameterContainer * cc){
                                 auto ccParent =dynamic_cast<ParameterContainer*>(cc->parentContainer);
                                 if(ccParent!=p && // if we are more than 2 level away
                                   ( dynamic_cast<NodeContainer*>(ccParent)|| // if parent is NodeContainer
                                    ccParent->presetable->currentPresetName->stringValue().isNotEmpty()) // parent has PresetLoaded
                                    ){
                                     return false;
                                 }
                                 return cc->canHavePresets();
                             },
                             -1,false,true,true);
    }
    return nullptr;
}

bool Presetable::saveCurrentPreset(void *notif)
{
    //Same as saveNewPreset because PresetManager now replaces if name is the same
    if (!hasPresetLoaded())
    {
        //        jassertfalse;
        return false;
    }

    Preset* pre = addNamedPreset(currentPresetName->stringValue(),true,notif);

    NLOG (pc->getControlAddress().toString(), juce::translate("Current preset saved : ") + pre->getPresetName());
    return pre!=nullptr;


}

int Presetable::getNumPresets()
{
    auto * pm = PresetManager::getInstance();
    return pm->getNumPresetForFilter (getPresetFilter());
}

bool Presetable::resetToCurrentPreset()
{
    if (!hasPresetLoaded()) return false;

    if(pc){
        pc->configureFromObject(currentPreset->getPresetValueObject());
    }
    else{
        jassertfalse;
    }

    return true;
}


bool Presetable::hasPresetLoaded(){
    return currentPreset!=nullptr;

}

void Presetable::cleanUpPresets()
{
    if(presetSync){
        delete presetSync;
        presetSync=nullptr;
    }
    currentPreset = nullptr;
    auto * pm = PresetManager::getInstance();
    pm->deletePresetsForContainer (pc, true);


}

String Presetable::getPresetFilter()
{
    if(pc){
        return getType()+"_" + pc->uid.toString();
    }
    else{
        return "";
    }
}

String Presetable::getType(){
    if(pc){
        String st = pc->getSubTypeName();
        return pc->getFactoryTypeId().toString()+(st.isEmpty()?"":String("_")+st);
    }
    else{
        return "";
    }
}


