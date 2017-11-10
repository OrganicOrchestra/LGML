/*
 ==============================================================================

 Copyright © Organic Orchestra, 2017

 This file is part of LGML. LGML is a software to manipulate sound in realtime

 This program is free software; you can redistribute it and/or modify
 it under the terms of the GNU General Public License as published by
 the Free Software Foundation (version 3 of the License).

 This program is distributed in the hope that it will be useful,
 but WITHOUT ANY WARRANTY; without even the implied warranty of
 MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.

 ==============================================================================
 */

#include "FastMapper.h"

#include "../Controller/ControllerManager.h"
juce_ImplementSingleton (FastMapper)
IMPL_OBJ_TYPE (FastMapper);


FastMapper::FastMapper (StringRef name) :
ParameterContainer (name),
autoAddFastMaps(false),
lastFMAddedTime(0)
{

    nameParam->isEditable = false;
    potentialIn = addNewParameter<ParameterProxy> ("Input", "potential input for new fastMap");
    potentialOut = addNewParameter<ParameterProxy> ("Output", "potential output for new fastMap");


    LGMLDragger::getInstance()->addSelectionListener (this);
    auto cm = ControllerManager::getInstance();
    cm->addControllableContainerListener(this);

    potentialIn->isSavable = false;
    potentialOut->isSavable = false;
    potentialIn->isPresettable = false;
    potentialOut->isPresettable = false;

}

FastMapper::~FastMapper()
{
    if (auto* dr = LGMLDragger::getInstanceWithoutCreating())
    {
        dr->removeSelectionListener (this);
    }
    if(auto cm = ControllerManager::getInstanceWithoutCreating()){
        cm->removeControllableContainerListener(this);
    }

    clear();
}



void FastMapper::setPotentialInput (Parameter* p)
{
    if(p!=potentialIn->get()){
        potentialIn->setParamToReferTo (p);
        createNewFromPotentials();
    }
}
void FastMapper::setPotentialOutput (Parameter* p )
{
    if(p!=potentialOut->get()){
        potentialOut->setParamToReferTo (p);
        createNewFromPotentials();
    }
}
void FastMapper::createNewFromPotentials()
{
    if (potentialIn->get() && potentialOut->get() && autoAddFastMaps)
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

    FastMap* f = new FastMap();
    addChildControllableContainer (f);
    f->nameParam->isEditable = true;
    maps.add (f);
    //    setContainerToListen (nullptr);
    f->referenceIn->setParamToReferTo (potentialIn->get());
    f->referenceOut->setParamToReferTo (potentialOut->get());

    potentialIn->setParamToReferTo (nullptr);
    potentialOut->setParamToReferTo (nullptr);
    if(!checkDuplicates (f)){
        lastFMAddedTime = Time::getMillisecondCounter();
    }
    return f;
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
            LOG ("!!! can't duplicate fastMap");
            removeFastmap (f);
            return true;

        }
    }

    return false;
}

void FastMapper::removeFastmap (FastMap* f)
{
    jassert (f);
    removeChildControllableContainer (f);
    maps.removeObject (f);
}


ParameterContainer*   FastMapper::addContainerFromObject (const String& /*name*/, DynamicObject*   /*fData*/)
{
    FastMap* f = addFastMap();

    return f;


}




void FastMapper::selectionChanged (Parameter* c )
{
    setPotentialOutput (Parameter::fromControllable (c));

};


void FastMapper::mappingModeChanged(bool state){
    autoAddFastMaps = state;
};



void FastMapper::controllableFeedbackUpdate (ControllableContainer* notif, Controllable* ori)
{
    ParameterContainer::controllableFeedbackUpdate (notif, ori);

    if (auto p = Parameter::fromControllable (ori))
    {
        if(notif== ControllerManager::getInstance()){
            auto now=Time::getMillisecondCounter();
            jassert(now>=lastFMAddedTime);
            // debounce control changes, to avoid setting potentialOutput back
            if (ori->isUserDefined && now-lastFMAddedTime>500){
                setPotentialInput (p);
            }
        }
        
        
    }
};
