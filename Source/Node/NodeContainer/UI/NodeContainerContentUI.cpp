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

#include "NodeContainerContentUI.h"
#include "../../Manager/UI/NodeManagerUI.h"

#include "../../../Controllable/Parameter/UI/ParameterUI.h"

#include "../../../UI/LayoutUtils.h"

NodeContainerContentUI::NodeContainerContentUI() :
    ConnectableNodeContentUI(),
    editContainerBT ("Edit Container"),
    addUserParamBT ("Add Variable")
{
    addAndMakeVisible (&editContainerBT);
    LGMLUIUtils::optionallySetBufferedToImage(&editContainerBT);
    editContainerBT.addListener (this);

    addAndMakeVisible (&addUserParamBT);
    addUserParamBT.addListener (this);

    setSize (250, 100);
    setPaintingIsUnclipped(true);

}

NodeContainerContentUI::~NodeContainerContentUI()
{
    
}

void NodeContainerContentUI::resized()
{

    Rectangle<int> r = getLocalBounds().reduced (5);

    editContainerBT.setBounds (r.removeFromTop (20));
    r.removeFromTop (5);
    addUserParamBT.setBounds (r.removeFromTop (20));

    layoutComponentsInGrid(userParamUI,r);



}




void NodeContainerContentUI::init()
{
    NodeContainer* nc = dynamic_cast<NodeContainer*> (node.get());

    if (!nc) {jassertfalse; return;}

    nodeContainer = nc;
    nodeContainer->userContainer.addControllableContainerListener(this);
    auto allP = nodeContainer->userContainer.getControllablesOfType<ParameterBase>(false);
    for(auto & p:allP){
        if(p->isUserDefined){
        childControllableAdded(&nodeContainer->userContainer, p.get());
        }
    }



}



void NodeContainerContentUI::buttonClicked (Button* b)
{
    if (b == &editContainerBT)
    {
        NodeManagerUI* nmui  = findParentComponentOfClass<NodeManagerUI>();

        if (nmui != nullptr)
        {
            nmui->setCurrentViewedContainer (nodeContainer);
        }
    }
    else if (b==&addUserParamBT){
        nodeContainer->userContainer.addNewParameter<FloatParameter> ("variable", "Custom Variable");

    }
}

void NodeContainerContentUI::childControllableAdded (ControllableContainer* cc, Controllable*c){
    jassert(nodeContainer && cc==&nodeContainer->userContainer);
    
    if(auto p = dynamic_cast<ParameterBase*>(c)){
        if(!p->isUserDefined){return;}
        auto added = userParamUI.add (ParameterUIFactory::createDefaultUI(p));
        addAndMakeVisible (added);
        resized();
    }
    else{
        jassertfalse;
    }
}

void NodeContainerContentUI::childControllableRemoved (ControllableContainer* cc, Controllable* c ){
    jassert(nodeContainer && cc==&nodeContainer->userContainer);
    if(auto p = dynamic_cast<ParameterBase*>(c)){
        if(!p->isUserDefined){return;}
        Array<ParameterUI*> toRm ;
        for(auto &pui : userParamUI){
            if(pui->parameter==c){toRm.add(pui);}
        }
        for(auto rm:toRm){
            userParamUI.removeObject(rm);
        }
        if(toRm.size()){
            resized();
        }


    }
    else{
        jassertfalse;
    }

}

#endif
