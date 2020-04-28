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

#if !ENGINE_HEADLESS && !NON_INCREMENTAL_COMPILATION

#include "JsNodeUI.h"

#include "JsNode.h"
#include "../../Controllable/Parameter/UI/ParameterUIFactory.h"

JsNodeUI::JsNodeUI(){};
JsNodeUI::~JsNodeUI()
{
    if (node.get())
    {
        node->removeControllableContainerListener(this);
        ((JsNode *)node.get())->jsParameters->scriptPath->removeFileListener(this);
    };
};

void JsNodeUI::init()
{
    JsNode *jsNode = (JsNode *)node.get();
    jsUI = std::make_unique<JsEnvironmentUI>(((JsEnvironment *)jsNode)->jsParameters.get());
    jsNode->addControllableContainerListener(this);

    addAndMakeVisible(jsUI.get());

    jsNode->jsParameters->scriptPath->addFileListener(this);

    if (jsNode->hasValidJsFile())
    {
        loadingEnded(nullptr);
    }

    setDefaultSize(350, 150);
};

void JsNodeUI::loadingEnded(FileParameter *)
{
    JsNode *jsNode = (JsNode *)node.get();
    varUI.clear();

    for (auto &c : jsNode->jsDynamicParameters)
    {
        childControllableAdded(jsNode, c);
    }
};

void JsNodeUI::resized()
{
    layoutUI();
}

void JsNodeUI::layoutUI()
{
    Rectangle<int> area = getLocalBounds().reduced(2);
    jsUI->setBounds(area.removeFromTop(30));

    for (auto &comp : varUI)
    {
        comp->setBounds(area.removeFromTop(20).reduced(2));
    }
}

void JsNodeUI::childControllableAdded(ControllableContainer *, Controllable *c)
{
    JsNode *jsNode = (JsNode *)node.get();

    if (!jsNode->jsDynamicParameters.contains((ParameterBase *)c))
        return;

    ParameterUI *comp = new NamedParameterUI(ParameterUIFactory::createDefaultUI(ParameterBase::fromControllable(c)), 100);
    varUI.add(comp);
    addAndMakeVisible(comp);

    layoutUI();
}
void JsNodeUI::childControllableRemoved(ControllableContainer *, Controllable *c)
{
    ParameterBase *pToComp = ParameterBase::fromControllable(c);
    WeakReference<Component> thisRef(this);
    MessageManager::callAsync([thisRef, pToComp]() {
        auto thisUI = dynamic_cast<JsNodeUI *>(thisRef.get());
        if (thisUI)
        {

            for (auto &comp : thisUI->varUI)
            {
                if (comp->parameter == pToComp)
                {
                    thisUI->removeChildComponent(comp);
                    thisUI->varUI.removeObject(comp);
                    break;
                }
            }

            thisUI->layoutUI();
            
        }
    });
};

#endif
