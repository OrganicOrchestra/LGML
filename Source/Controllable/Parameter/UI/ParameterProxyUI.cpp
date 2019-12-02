/* Copyright © Organic Orchestra, 2017
*
* This file is part of LGML.  LGML is a software to manipulate sound in realtime
*
* This program is free software; you can redistribute it and/or modify
* it under the terms of the GNU General Public License as published by
* the Free Software Foundation (version 3 of the License).
*
* This program is distributed in the hope that it will be useful,
* but WITHOUT ANY WARRANTY; without even the implied warranty of
* MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
*
*/
#if !ENGINE_HEADLESS

#include "ParameterProxyUI.h"
#include "StringParameterUI.h"
#include "../../ControllableContainer.h"
#include "ParameterUIFactory.h"

ParameterProxyUI::ParameterProxyUI (ParameterProxy* proxy) :
    linkedParamUI (nullptr),
    ParameterUI (proxy),
    paramProxy (proxy),
    chooser (proxy->getRoot())

{
    setMappingDest (true);
    chooser.addControllableReferenceUIListener (this);
    addAndMakeVisible (&chooser);
    chooser.setPaintingIsUnclipped(true);
    if(paramProxy){
        paramProxy->addParameterProxyListener (this);
        chooser.setTooltip(juce::translate(paramProxy->description));
        setLinkedParamUI (paramProxy->linkedParam);
    }
    else{
        jassertfalse;
    }
}

ParameterProxyUI::~ParameterProxyUI()
{
    if(paramProxy.get()){
        paramProxy->removeParameterProxyListener (this);
    }
//    setLinkedParamUI (nullptr);
}

void ParameterProxyUI::resized()
{
    Rectangle<int> r = getLocalBounds();

    if (r.getWidth() == 0 || r.getHeight() == 0) return;

    if (linkedParamUI != nullptr)
    {
        chooser.setBounds (r.removeFromLeft(10));
        linkedParamUI->setBounds (r);

    }
    else {
        chooser.setBounds(r);

    }

}

void ParameterProxyUI::setLinkedParamUI ( ParameterBase* p)
{


    if (linkedParamUI != nullptr)
    {
        //    auto * cUI = dynamic_cast<ParameterUI*>(linkedParamUI->ownedParameterUI.get());
        auto* cUI = dynamic_cast<ParameterUI*> (linkedParamUI.get());

        if (cUI && p && cUI->parameter == p) return;

        removeChildComponent (linkedParamUI.get());
        linkedParamUI = nullptr;
    }



    //  linkedParamUI = p?new NamedParameterUI(ParameterUIFactory::createDefaultUI(p),100,true):nullptr;
    if (auto prox = dynamic_cast<ParameterProxy*> (p))
    {
        //encapsulate ui if proxy of proxy to show it explicitly
        linkedParamUI =  std::make_unique< NamedParameterUI> (ParameterUIFactory::createDefaultUI (prox->linkedParam), 20);
//        linkedParamUI->controllableLabel.setText ("proxy : " + p->niceName, dontSendNotification);

    }
    else
    {
        linkedParamUI = p ? ParameterUIFactory::createDefaultUI (p): nullptr;
    }


    if (linkedParamUI != nullptr)
    {
        addAndMakeVisible (linkedParamUI.get());
    }

//    chooser.setVisible (linkedParamUI == nullptr);
    chooser.setCurrentControllable(p);
    if((linkedParamUI==nullptr )&& parameter ){
        String path = parameter->stringValue();
        if(!path.isEmpty()){
            chooser.setButtonText(path);
        }
    }
    chooser.filterOutControllable = {paramProxy};
    resized();
}



void ParameterProxyUI::linkedParamChanged (ParameterProxy* p)
{
    jassert (p == paramProxy);

    if (!MessageManager::getInstance()->isThisTheMessageThread())
    {
        WeakReference<ParameterProxy> wfp (p);
        MessageManager::getInstance()->callAsync ([this, wfp]() {
            if(wfp.get()){
            linkedParamChanged (wfp.get());
            }

        });
    }
    else
    {
        setLinkedParamUI (p->linkedParam);
    }
}

void ParameterProxyUI::choosedControllableChanged (ControllableReferenceUI*, Controllable* c)
{
    auto t = ParameterBase::fromControllable (c);
    paramProxy->setParamToReferTo (t);


}

//void ParameterProxyUI::controllableNameChanged (Controllable* c)
//{
//    ParameterUI::controllableNameChanged (c);
//
//
//}

#endif
