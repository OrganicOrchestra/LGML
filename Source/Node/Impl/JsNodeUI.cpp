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
#include "JsNodeUI.h"

#include "JsNode.h"
#include "../../Controllable/Parameter/UI/ParameterUIFactory.h"

JsNodeUI::JsNodeUI() {};
JsNodeUI::~JsNodeUI()
{
    if ( node.get())
    {
        node->removeControllableContainerListener (this);
        ((JsNode*)node.get())->JsEnvironment::removeListener (this);
    };

};

void JsNodeUI::init()
{
    JsNode* jsNode = (JsNode*) node.get();
    jsUI = new JsEnvironmentUI (((JsEnvironment*)jsNode)->jsParameters);
    jsNode->addControllableContainerListener (this);

    addAndMakeVisible (jsUI);


    jsNode->JsEnvironment::addListener (this);

    if (jsNode->hasValidJsFile())
    {
        newJsFileLoaded (true);
    }

    setDefaultSize (350, 150);

};

void JsNodeUI::newJsFileLoaded (bool v)
{
    JsNode* jsNode = (JsNode*) node.get();
    varUI.clear();

//    if (v)
//    {
        for (auto& c : jsNode->jsDynamicParameters)
        {
            controllableAdded (jsNode, c);
        }
//    }
};

void JsNodeUI::resized()
{
    layoutUI();
}

void JsNodeUI::layoutUI()
{
    Rectangle<int> area = getLocalBounds().reduced (2);
    jsUI->setBounds (area.removeFromTop (30));

    for (auto& comp : varUI)
    {
        comp->setBounds ( area.removeFromTop (20).reduced (2));
    }

}

void JsNodeUI::controllableAdded (ControllableContainer*, Controllable* c)
{
    JsNode* jsNode = (JsNode*) node.get();

    if (!jsNode->jsDynamicParameters.contains ((Controllable*)c))return;

    ParameterUI* comp = new NamedParameterUI (ParameterUIFactory::createDefaultUI ( ParameterBase::fromControllable (c)), 100);
    varUI.add (comp);
    addAndMakeVisible (comp);

    if (varUI.size() * 20 > getHeight() - jsUI->getHeight())
    {
        setSize ( getWidth(), varUI.size() * 20 + 5 + jsUI->getHeight());
    }
    else
    {
        layoutUI();
    }

}
void JsNodeUI::controllableRemoved (ControllableContainer*, Controllable* c)
{
    ParameterBase* pToComp = ParameterBase::fromControllable (c);

    for (auto& comp : varUI)
    {
        if (comp->parameter == pToComp)
        {
            removeChildComponent (comp);
            varUI.removeObject (comp);
            break;
        }

    }


    layoutUI();


};

#endif
