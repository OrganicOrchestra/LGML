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

#include "FastMapperUI.h"
#include "FastMapper.h"
#include "../Controllable/Parameter/UI/ParameterUIFactory.h"

FastMapperUI::FastMapperUI (FastMapper* _fastMapper, ControllableContainer* _viewFilterContainer) :
    fastMapper (_fastMapper), viewFilterContainer (_viewFilterContainer)
{
    fastMapper->addControllableContainerListener (this);

    linkToSelection.setButtonText ("Show from selected");
    linkToSelection.setTooltip ("filter viewed fastmap to currently selected element (Node / Controller ...)");
    linkToSelection.setClickingTogglesState (true);
    linkToSelection.addListener (this);
    addAndMakeVisible (linkToSelection);

    



    potentialIn =  ParameterUIFactory::createDefaultUI (fastMapper->potentialIn);
    potentialOut = ParameterUIFactory::createDefaultUI (fastMapper->potentialOut);
    candidateLabel.setText("candidate", dontSendNotification);
    addAndMakeVisible(candidateLabel);
    addAndMakeVisible (potentialIn);
    addAndMakeVisible (potentialOut);
    resetAndUpdateView();


}

FastMapperUI::~FastMapperUI()
{
    fastMapper->removeControllableContainerListener (this);
    clear();


}

void FastMapperUI::addFastMapUI (FastMap* f)
{
    FastMapUI* fui = new FastMapUI (f);
    mapsUI.add (fui);
    addAndMakeVisible (fui);
    fastMapperUIListeners.call (&FastMapperUIListener::fastMapperContentChanged, this);
}

void FastMapperUI::removeFastMapUI (FastMapUI* fui)
{

    if (fui == nullptr) return;

    removeChildComponent (fui);
    mapsUI.removeObject (fui);

    fastMapperUIListeners.call (&FastMapperUIListener::fastMapperContentChanged, this);
}


void FastMapperUI::resetAndUpdateView()
{

    clear();

    for (auto& f : fastMapper->maps)
    {
        if (mapPassViewFilter (f)) addFastMapUI (f);
    }

    resized();
}

void FastMapperUI::setViewFilter (ControllableContainer* filterContainer)
{
    viewFilterContainer = filterContainer;
    viewFilterControllable = nullptr;
    MessageManager::getInstance()->callAsync([this](){resetAndUpdateView();});
}

void FastMapperUI::setViewFilter (Controllable* filterControllable)
{
    viewFilterContainer = nullptr;
    viewFilterControllable = filterControllable;
    MessageManager::getInstance()->callAsync([this](){resetAndUpdateView();});
}

void FastMapperUI::resetViewFilter(){
    viewFilterContainer = nullptr;
    viewFilterControllable = nullptr;
    MessageManager::getInstance()->callAsync([this](){resetAndUpdateView();});
}

bool FastMapperUI::mapPassViewFilter (FastMap* f)
{

    if (viewFilterContainer == nullptr && viewFilterControllable==nullptr) return true;

    if( viewFilterContainer){
    if (f->referenceIn->linkedParam != nullptr && (ControllableContainer*)f->referenceIn->linkedParam->isChildOf (viewFilterContainer)) return true;

    if (f->referenceOut->linkedParam != nullptr && (ControllableContainer*)f->referenceOut->linkedParam->isChildOf (viewFilterContainer)) return true;
    }
    else if(viewFilterControllable){
        if (f->referenceIn->linkedParam == viewFilterControllable ||
            f->referenceOut->linkedParam == viewFilterControllable)
            return true;
    }

    // pass through emptys
    return (!f->referenceOut->linkedParam && !f->referenceIn->linkedParam);
}



FastMapUI* FastMapperUI::getUIForFastMap (FastMap* f)
{
    for (auto& fui : mapsUI)
    {
        if (fui->fastMap == f) return fui;
    }

    return nullptr;
}

constexpr int buttonHeight = 21;
int FastMapperUI::getContentHeight() const
{

    return mapsUI.size() * (mapHeight + gap) + 2 * buttonHeight + 10;
}

void FastMapperUI::resized()
{
    Rectangle<int> r = getLocalBounds().reduced (2);
    auto inputHeader  = r.removeFromTop (buttonHeight);
    candidateLabel.setBounds(inputHeader.removeFromLeft(100));
    potentialIn->setBounds (inputHeader.removeFromLeft(inputHeader.getWidth()/2).reduced (2));
    potentialOut->setBounds (inputHeader.reduced (2));
    linkToSelection.setBounds (r.removeFromTop (buttonHeight).reduced (2));


    r.removeFromTop (10);
    r.reduce (2, 0);

    for (auto& fui : mapsUI)
    {
        fui->setBounds (r.removeFromTop (mapHeight));
        r.removeFromTop (gap);
    }
}

void FastMapperUI::clear()
{
    while (mapsUI.size() > 0)
    {
        removeFastMapUI (mapsUI[0]);
    }
}

void FastMapperUI::mouseDown (const MouseEvent& e)
{
    if (e.mods.isRightButtonDown())
    {
        PopupMenu m;
        m.addItem (1, "Add Fast Map");
        int result = m.show();

        switch (result)
        {
            case 1:
                fastMapper->addFastMap();
                break;
        }
    }
}


void FastMapperUI::controllableContainerAdded (ControllableContainer* ori, ControllableContainer* cc)
{
    if (ori == fastMapper)
    {

        WeakReference<ControllableContainer> wf (cc);
        MessageManager::callAsync ([this, wf] ()
        {
            if (wf.get())
            {
                addFastMapUI ((FastMap*)wf.get());
                resized();
            }
        });
    }

    //    addFastMapUI(f);
    //    resized();
}

void FastMapperUI::controllableContainerRemoved (ControllableContainer* ori, ControllableContainer* cc)
{
    if (ori == fastMapper)
    {

        WeakReference<Component> fui (getUIForFastMap ((FastMap*)cc));
        execOrDefer ([ = ]()
        {
            if (fui.get())
            {
                removeFastMapUI (dynamic_cast<FastMapUI*> (fui.get()));
                resized();
            }
        });


    }
}

void FastMapperUI::buttonClicked (Button* b)
{
    if (b == &linkToSelection)
    {
        if (linkToSelection.getToggleState())
        {
            Inspector::getInstance()->addInspectorListener (this);
            setViewFilter (Inspector::getInstance()->getCurrentContainerSelected());
        }
        else
        {
            Inspector::getInstance()->removeInspectorListener (this);
            resetViewFilter ();
        }
    }


}

ParameterUI * getParamFromComponent(Component * comp){
    if(!comp) return nullptr;
    if(auto pui = dynamic_cast<ParameterUI*>(comp))return pui;
    for(auto c:comp->getChildren()){
        if(auto pui = dynamic_cast<ParameterUI*>(c))return pui;
    }
    return nullptr;
}
void FastMapperUI::currentComponentChanged (Inspector* i)
{
    jassert (linkToSelection.getToggleState());
    auto * curCont = i->getCurrentContainerSelected();
    // prevent self selection
    if (dynamic_cast<FastMap*> (curCont) || dynamic_cast<FastMapper*>(curCont)) {return;}

    if(auto container = curCont)
        setViewFilter (container);
    else if(auto pui = getParamFromComponent(i->getCurrentComponent())){
        setViewFilter(pui->parameter);

    }


};

void FastMapperViewport::buttonClicked (Button* b)
{
    if (b == &addFastMapButton )
    {
        fastMapperUI->fastMapper->addFastMap();
    }

}
