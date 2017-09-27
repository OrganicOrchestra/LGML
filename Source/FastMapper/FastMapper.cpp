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

juce_ImplementSingleton (FastMapper)
IMPL_OBJ_TYPE (FastMapper);

FastMapper::FastMapper (StringRef name) :
    ParameterContainer (name),
    selectedContainerToListenTo (nullptr),
    autoAdd(false)
{

    nameParam->isEditable = false;
    potentialIn = addNewParameter<ParameterProxy> ("potential Input", "potential input for new fastMap");
    potentialOut = addNewParameter<ParameterProxy> ("potential Output", "potential output for new fastMap");
    

    LGMLDragger::getInstance()->addSelectionListener (this);
    Inspector::getInstance()->addInspectorListener (this);
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

    if (auto* i = Inspector::getInstanceWithoutCreating())
    {
        i->removeInspectorListener (this);
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
    if (potentialIn->get() && potentialOut->get() && autoAdd)
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
    checkDuplicates (f);
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
    setPotentialInput (Parameter::fromControllable (c));

};


void FastMapper::mappingModeChanged(bool state){
    autoAdd = state;
};

void FastMapper::currentComponentChanged (Inspector* i)
{
    auto* newC = i->getCurrentSelected();

    if (newC == selectedContainerToListenTo)return;

    setContainerToListen (newC);

};

void FastMapper::setContainerToListen (ControllableContainer* newC)
{
    if (selectedContainerToListenTo)
    {
        selectedContainerToListenTo->removeControllableContainerListener (this);
    }

    selectedContainerToListenTo = newC;

    if (selectedContainerToListenTo)
    {
        selectedContainerToListenTo->addControllableContainerListener (this);
    }
}
void FastMapper::controllableFeedbackUpdate (ControllableContainer* notif, Controllable* ori)
{
    ParameterContainer::controllableFeedbackUpdate (notif, ori);

    if (auto p = Parameter::fromControllable (ori))
    {
        if (notif == selectedContainerToListenTo && p->isEditable && LGMLDragger::getInstance()->isMappingActive)
        {
            setPotentialOutput (p);
        }
    }
};
