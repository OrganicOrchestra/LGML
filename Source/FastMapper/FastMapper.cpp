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

#include "FastMapper.h"

#if !ENGINE_HEADLESS
#include "../Controllable/Parameter/UI/ParameterUI.h"
#endif

#include "../Controller/ControllerManager.h"
juce_ImplementSingleton (FastMapper)
IMPL_OBJ_TYPE (FastMapper);

template<>
void ControllableContainer::OwnedFeedbackListener<FastMapper>::controllableFeedbackUpdate (ControllableContainer* notif, Controllable* ori) {

    if (auto p = ParameterBase::fromControllable (ori))
    {
        if(owner->autoAddFastMaps){
            if(notif== ControllerManager::getInstance()){
                auto now=Time::getMillisecondCounter();
                jassert(now>=owner->lastFMAddedTime);
                // debounce control changes, to avoid setting potentialOutput back
                if (p->isUserDefined && now-owner->lastFMAddedTime>500){
                    owner->setPotentialInput (p);
                }
            }
        }


    }
}



FastMapper::FastMapper (StringRef name) :
ParameterContainer (name),
autoAddFastMaps(false),
lastFMAddedTime(0),
pSync(this)
{
    canHaveUserDefinedContainers = true;
    nameParam->setInternalOnlyFlags(true,false);
    potentialIn = addNewParameter<ParameterProxy> ("Input", "potential input for new fastMap,\nto assing :\n- move a controller in mapping mode\n- alt click on other LGML parameter\n- use this popup");
    potentialOut = addNewParameter<ParameterProxy> ("Output", "potential output for new fastMap\nto assign :\n- click on parameter in mapping mode\n- navigate through this popup");

    potentialIn->addParameterProxyListener(this);
    potentialOut->addParameterProxyListener(this);
#if !ENGINE_HEADLESS
    LGMLDragger::getInstance()->addSelectionListener (this);
#endif

    potentialIn->setSavable(false);
    potentialOut->setSavable(false);
    


}

FastMapper::~FastMapper()
{
#if !ENGINE_HEADLESS
    if (auto* dr = LGMLDragger::getInstanceWithoutCreating())
    {
        dr->removeSelectionListener (this);
    }
#endif
    if(auto cm = ControllerManager::getInstanceWithoutCreating()){
        cm->removeControllableContainerListener(&pSync);
    }

    clear();
}



void FastMapper::setPotentialInput ( ParameterBase* p)
{
    if(p!=potentialIn->get()){
        potentialIn->setParamToReferTo (p);

    }
}
void FastMapper::setPotentialOutput ( ParameterBase* p )
{
    if(p!=potentialOut->get()){
        potentialOut->setParamToReferTo (p);
    }
}
void FastMapper::createNewFromPotentials()
{
    if (potentialIn->get() && potentialOut->get() )
    {
        addFastMap();

    }
}
void FastMapper::clear()
{
    while (maps.size())
    {
        removeFastmap (maps[0]);
    }

    potentialOut->setParamToReferTo (nullptr);
    potentialIn->setParamToReferTo (nullptr);

}

FastMap* FastMapper::addFastMap()
{


    ScopedPointer<FastMap> f ( new FastMap());

    f->nameParam->isEditable = true;

    
    f->referenceIn->setParamToReferTo (potentialIn->get());
    f->referenceOut->setParamToReferTo (potentialOut->get());


    if(!checkDuplicates (f)){
        addChildControllableContainer (f);
        maps.add (f);
        f->referenceIn->addParameterProxyListener(this);
        f->referenceOut->addParameterProxyListener(this);

        lastFMAddedTime = Time::getMillisecondCounter();

#if !ENGINE_HEADLESS
        auto addedFastMap = f.release();
        WeakReference<FastMap> wkf(addedFastMap);
        // avoid listener feedback
        MessageManager::callAsync([this,wkf](){
            if(!wkf.get()){return;}
            auto pIns = wkf->referenceIn->linkedParam?ParameterUI::getAllParameterUIs().getForParameter(wkf->referenceIn->linkedParam):AllParamType::ArrayType();
            auto pOuts = wkf->referenceOut->linkedParam?ParameterUI::getAllParameterUIs().getForParameter(wkf->referenceOut->linkedParam):AllParamType::ArrayType();

            for(auto p : pIns){p->setHasMappedParameter(true);}
            for(auto p : pOuts){p->setHasMappedParameter(true);}

            if(auto dr = LGMLDragger::getInstance()){
                dr->setSelected(nullptr,this);
            }
            potentialIn->setParamToReferTo (nullptr);
            potentialOut->setParamToReferTo (nullptr);
        });
#endif
        return addedFastMap;
    }
    else{
        return nullptr;
    }
}

bool FastMapper::checkDuplicates (FastMap* f)
{
    bool dup = false;

    if (f->referenceIn->get() == nullptr && f->referenceOut->get() == nullptr)
        return false;

    for (auto& ff : maps)
    {
        if (ff == f)continue;

        if (ff->referenceIn->get() == f->referenceIn->get() &&
            ff->referenceOut->get() == f->referenceOut->get())
        {
            dup = true;
        }
        else if (ff->referenceIn->get() == f->referenceOut->get() &&
                 ff->referenceOut->get() == f->referenceIn->get())
        {
            dup = true;
        }

        if (dup)
        {
            LOGE(juce::translate("can't duplicate fastMap"));
            removeFastmap (f);
            return true;

        }
    }

    return false;
}

void FastMapper::removeFastmap (FastMap* f)
{
    f->referenceIn->setParamToReferTo(nullptr);
    f->referenceOut->setParamToReferTo(nullptr);
    jassert (f);
    removeChildControllableContainer (f);
    maps.removeObject (f);
}


ParameterContainer*   FastMapper::addContainerFromObject (const String& /*name*/, DynamicObject*   fData)
{
    FastMap* f = addFastMap();
    if(f){
        f->configureFromObject(fData);
    }
    return f;


}



#if !ENGINE_HEADLESS
void FastMapper::selectionChanged ( ParameterBase* c )
{

    auto ms = Desktop::getInstance().getMouseSource(0);
    if (ms&& ms->getCurrentModifiers().isAltDown()){
        setPotentialInput ( ParameterBase::fromControllable (c));
    }
    else{
        setPotentialOutput ( ParameterBase::fromControllable (c));
    }

};


void FastMapper::mappingModeChanged(bool state){
    autoAddFastMaps = state;
    if(auto cm = ControllerManager::getInstance()){
        if(state){
            cm->addControllableContainerListener(&pSync);
        }
        else{
            cm->removeControllableContainerListener(&pSync);
            setPotentialInput(nullptr);
            setPotentialOutput(nullptr);
        }
    }


};

#endif
void  FastMapper::linkedParamChanged (ParameterProxy* p ) {
    if(p== potentialIn || p== potentialOut){
        createNewFromPotentials();
    }
    else{
#if !ENGINE_HEADLESS
        // remove old mapped uis
        String lastAddressS = p->lastValue;
        if(lastAddressS.isNotEmpty()){
            ControlAddressType lastAddress =  ControlAddressType::fromString(lastAddressS);
            auto lastP = dynamic_cast<ParameterBase*>(ControllableContainer::getRoot(true)->getControllableForAddress(lastAddress));
            if(lastP){
                bool isStillMapped=false;
                for(auto fm : maps){
                    if(fm->getProxyForParameter(lastP)){
                        isStillMapped = true;
                        break;
                    }
                }
                if(!isStillMapped){
                    auto & allUis (ParameterUI::getAllParameterUIs());
                    auto pUIs = allUis.getForParameter(lastP);
                    for(auto ui:pUIs){
                        ui->setHasMappedParameter(false);
                    }
                }
            }
        }

        // add newMapped uis
        String addrS = p->stringValue();
        if(addrS.isNotEmpty()){
            ControlAddressType addr =  ControlAddressType::fromString(addrS);
            auto lastP = dynamic_cast<ParameterBase*>(ControllableContainer::getRoot(true)->getControllableForAddress(addr));
            if(lastP){
                auto pUIs = ParameterUI::getAllParameterUIs().getForParameter(lastP);
                for(auto ui:pUIs){
                    ui->setHasMappedParameter(true);
                }
            }
        }


#endif
    }

}

bool  FastMapper::isParameterMapped (ParameterBase * p){
    if(!p)return false;
    //    jassert(p);
    for (const auto * f : maps){
        if(f->getProxyForParameter(p)){
            return true;
        }
    }

    return false;
}
