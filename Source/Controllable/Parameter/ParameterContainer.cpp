/*
 ==============================================================================

 ParameterContainer.cpp
 Created: 5 Sep 2017 3:59:43pm
 Author:  Martin Hermant

 ==============================================================================
 */

#include "ParameterContainer.h"
#include "../Parameter/ParameterFactory.h"

//#include "../Parameter/NumericParameter.h"
//#include "../Parameter/BoolParameter.h"
//#include "../Parameter/StringParameter.h"
//#include "../Parameter/EnumParameter.h"
//#include "../Parameter/RangeParameter.h"
//#include "../Parameter/ParameterList.h"
//#include "../Parameter/Trigger.h"
const Identifier ParameterContainer::presetIdentifier ("preset");
const Identifier ParameterContainer::uidIdentifier ("uid");

IMPL_OBJ_TYPE (ParameterContainer)

ParameterContainer::ParameterContainer (StringRef niceName):
ControllableContainer (niceName),
currentPreset (nullptr),
isHidenInEditor(false)
{



    nameParam = addNewParameter<StringParameter> ("Name", "Set the visible name", "");
    nameParam->isPresettable = false;



    currentPresetName = addNewParameter<StringParameter> (presetIdentifier.toString(), "Current Preset", "");
    currentPresetName->isHidenInEditor = true;
    savePresetTrigger = addNewParameter<Trigger> ("Save Preset", "Save current preset");
    savePresetTrigger->isHidenInEditor = true;


    // init once all are created to avoid uninitialized values
    nameParam->setValue (String (niceName));

}

ParameterContainer::~ParameterContainer()
{
    ParameterContainer::masterReference.clear();
}
const String  ParameterContainer::getNiceName()
{
    return nameParam->stringValue();
}

String ParameterContainer::setNiceName (const String& n)
{
    auto targetName = ControllableContainer::setNiceName (n);
    nameParam->setValue (targetName, targetName == getNiceName(), true);
    return targetName;
}


Array<WeakReference<ParameterBase>> ParameterContainer::getAllParameters (bool recursive, bool getNotExposed)
{
    Array<WeakReference<ParameterBase>> result;

    for (auto& c : controllables)
    {
        if (getNotExposed || c->isControllableExposed)
        {
            if ( ParameterBase* cc = ParameterBase::fromControllable (c))
            {
                result.add (cc);
            }
        }
    }

    if (recursive)
    {
        ScopedLock lk (controllableContainers.getLock());

        for (auto& cc : controllableContainers)
        {
            if (cc.get())
                result.addArray (((ParameterContainer*)cc.get())->getAllParameters (true, getNotExposed));
        }
    }

    return result;
}


DynamicObject* ParameterContainer::getObject()
{

    DynamicObject* data = new DynamicObject();
    data->setProperty (factoryTypeIdentifier, getFactoryTypeName());
    data->setProperty (uidIdentifier, uid.toString());
    {
        var paramsData (new DynamicObject);

        for (auto& c : controllables)
        {
            if (c->isUserDefined || c->shouldSaveObject  )
            {

                paramsData.getDynamicObject()->setProperty (c->shortName, ParameterFactory::createTypedObjectFromInstance ( ParameterBase::fromControllable (c)));
            }
            else if (c->isSavable)
            {
                paramsData.getDynamicObject()->setProperty (c->shortName, c->getVarState());
            }
        }


        data->setProperty (controllablesId, paramsData);

    }


    if (controllableContainers.size())
    {
        DynamicObject*   childData = new DynamicObject();

        for (auto controllableCont : controllableContainers)
        {
            childData->setProperty (controllableCont->shortName, controllableCont.get()->getObject());
        }

        data->setProperty (childContainerId, childData);
    }



    return data;
}

void ParameterContainer::containerWillClear (ControllableContainer* c)
{
    if (c == this)cleanUpPresets();
}

void ParameterContainer::controllableRemoved (ControllableContainer*, Controllable* c)
{

    if (auto p = ParameterBase::fromControllable (c))
    {
        p->removeParameterListener (this);
        p->removeAsyncParameterListener (this);
    }
}
ParameterBase*   ParameterContainer::addParameter ( ParameterBase* p)
{

    p->setParentContainer (this);
    controllables.add (p);
    controllableContainerListeners.call (&ControllableContainerListener::controllableAdded, this, p);
    notifyStructureChanged (this,true);
    addControllableInternal (p);
    p->addParameterListener (this);
    p->addAsyncParameterListener (this);
    p->isUserDefined = isUserDefined;
    return p;


}



void ParameterContainer::newMessage (const ParameterBase::ParamWithValue& pv)
{
    
    if (pv.parameter == currentPresetName)
    {
        loadPresetWithName (pv.parameter->stringValue());
    }

    if (!pv.isRange())
    {
        onContainerParameterChangedAsync (pv.parameter, pv.value);
    }
}

void ParameterContainer::parameterValueChanged ( ParameterBase* p, ParameterBase::Listener * notifier)
{
    if(notifier!=this){
        if (p == nameParam)
        {
            if (parentContainer)
            {
                String oN = nameParam->stringValue();
                String tN = parentContainer->getUniqueNameInContainer (oN, 0, this);

                if (tN != oN)
                {
                    nameParam->setValue (tN, false, true);
                }
            }

            setAutoShortName();
        }
        else   if (p == savePresetTrigger)
        {
            saveCurrentPreset();

        }


        if (p && p->getFactoryTypeId() == Trigger::_factoryType)
        {
            onContainerTriggerTriggered ((Trigger*)p);
        }
        else
        {
            onContainerParameterChanged (p);
        }
    }

    if ( (p != nullptr && p->parentContainer == this && p->isControllableExposed ) ) dispatchFeedback (p);
}

void ParameterContainer::configureFromObject (DynamicObject* dyn)
{
    if (dyn)
    {

        if (dyn->hasProperty (uidIdentifier)) uid = dyn->getProperty (uidIdentifier);

        {
            DynamicObject* paramsData = dyn->getProperty (controllablesId).getDynamicObject();
            jassert (paramsData);

            if (paramsData != nullptr)
            {
                auto props = paramsData->getProperties();

                for (auto& p : props)
                {
                    if (Controllable* c = getControllableByName (p.name.toString(), true))
                    {
                        if (c->isSavable)
                        {
                            if ( ParameterBase* par = ParameterBase::fromControllable (c))
                            {
                                // we don't load preset when already loading a state
                                if (par->shortName != presetIdentifier.toString() )
                                {
                                    par->setValue (p.value);
                                }
                            }
                            else
                            {
                                // we don't use custom types for now
                                jassertfalse;
                            }
                        }
                    }
                    else
                    {
                        addParameterFromVar (p.name.toString(), p.value);
                    }

                }
            }
        }

        {


            auto cD = dyn->getProperty (childContainerId).getDynamicObject();

            if (cD)
            {
                auto ob = cD->getProperties();

                for (auto& o : ob)
                {
                    auto cont = dynamic_cast<ParameterContainer*> (getControllableContainerByName (o.name.toString()));

                    if (cont)
                    {
                        cont->configureFromObject (o.value.getDynamicObject());
                    }
                    else if(o.value.getDynamicObject())
                    {
                        auto c = addContainerFromObject (o.name.toString(), o.value.getDynamicObject());

                        if (c)
                        {
//                            c->configureFromObject (o.value.getDynamicObject());
                        }
                        else
                        {
                            jassertfalse;
                        }
                    }
                    else{
                        DBG("ignoring container : "+o.name.toString());
                    }
                }

            }
        }

    }
    else
    {
        jassertfalse;
    }




}


ParameterContainer*   ParameterContainer::addContainerFromObject (const String& name, DynamicObject*   /*data*/)
{
    auto res = new ParameterContainer (name);
    res->setUserDefined (true);
    addChildControllableContainer (res);
    return res;;
};
ParameterBase* ParameterContainer::addParameterFromVar (const String& name, const var& data)
{
    // handle automagically for userdefined
    if (isUserDefined)
    {
        if (data.isObject())
        {
            return addParameter (ParameterFactory::createBaseFromObject ( name, data.getDynamicObject()));
        }
        else
        {
            return addParameter (ParameterFactory::createBaseFromVar (name, data));
        }
    }
    else
    {
        //    jassertfalse;
        return nullptr;
    }
};

////////////
// Preset



bool ParameterContainer::loadPresetWithName (const String& name)
{
    // TODO weird feedback when loading preset on parameter presetName
    if (isLoadingPreset) {return false;}

    if (name == "") return false;

    isLoadingPreset = true;

    PresetManager::Preset* preset = PresetManager::getInstance()->getPreset (getPresetFilter(), name);

    if (preset == nullptr) {
        isLoadingPreset = false;
        currentPresetName->setValue ("", true, true);
        return false;}

    bool hasLoaded = loadPreset (preset);
    isLoadingPreset = false;
    return hasLoaded;

}

bool ParameterContainer::loadPreset (PresetManager::Preset* preset)
{
    if (preset == nullptr)
    {
        currentPresetName->setValue ("", true);
        return false;
    }

    loadPresetInternal (preset);

    for (auto& pv : preset->presetValues)
    {

        ParameterBase* p = dynamic_cast <ParameterBase*> (getControllableForAddress (pv->paramControlAddress));

        //DBG("Load preset, param set container : " << niceName << ", niceName : " << p->niceName << ",pv controlAddress : " << p->controlAddress << "" << pv->presetValue.toString());
        if (p != nullptr && p != currentPresetName) p->setValue (pv->presetValue);
    }

    currentPreset = preset;
    currentPresetName->setValue (currentPreset->name, false);

    controllableContainerListeners.call (&ControllableContainerListener::controllableContainerPresetLoaded, this);
    return true;
}

PresetManager::Preset* ParameterContainer::saveNewPreset (const String& _name)
{
    PresetManager::Preset* pre = PresetManager::getInstance()->addPresetFromControllableContainer (_name, getPresetFilter(), this, presetSavingIsRecursive);
    savePresetInternal (pre);
    NLOG (getNiceName(), juce::translate("New preset saved : ") + pre->name);
    loadPreset (pre);
    return pre;
}


bool ParameterContainer::saveCurrentPreset()
{
    //Same as saveNewPreset because PresetManager now replaces if name is the same
    if (currentPreset == nullptr)
    {
//        jassertfalse;
        return false;
    }

    PresetManager::Preset* pre = PresetManager::getInstance()->addPresetFromControllableContainer (currentPreset->name, getPresetFilter(), this, presetSavingIsRecursive);
    savePresetInternal (pre);
    NLOG (getNiceName(), juce::translate("Current preset saved : ") + pre->name);
    return loadPreset (pre);

    /*
     for (auto &pv : currentPreset->presetValues)
     {
ParameterBase* p = dynamic_cast <ParameterBase*> (getControllableForAddress(pv->paramControlAddress));
     if (p != nullptr && p!=currentPresetName)
     {
     pv->presetValue = var(p->value);
     }
     }
     savePresetInternal(currentPreset);
     NLOG(niceName, juce::translate("Current preset saved : ") + currentPreset->name);

     return true;
     */
}

int ParameterContainer::getNumPresets()
{
    return PresetManager::getInstance()->getNumPresetForFilter (getPresetFilter());
}

bool ParameterContainer::resetFromPreset()
{
    if (currentPreset == nullptr) return false;


    for (auto& pv : currentPreset->presetValues)
    {
        ParameterBase* p = ( ParameterBase*)getControllableForAddress (pv->paramControlAddress);

        if (p != nullptr && p != currentPresetName) p->resetValue();
    }


    currentPreset = nullptr;
    currentPresetName->setValue ("", true);

    return true;
}

var ParameterContainer::getPresetValueFor ( ParameterBase* p)
{
    if (currentPreset == nullptr) return var();
    
    return currentPreset->getPresetValue (p->getControlAddress (this));
}


void ParameterContainer::cleanUpPresets()
{
    PresetManager* pm = PresetManager::getInstanceWithoutCreating();
    
    if (pm != nullptr) pm->deletePresetsForContainer (this, true);
    
}

String ParameterContainer::getPresetFilter()
{
    return shortName;
}


void ParameterContainer::setUserDefined (bool v)
{
    ControllableContainer::setUserDefined (v);
    nameParam->isEditable = v;
    
}

