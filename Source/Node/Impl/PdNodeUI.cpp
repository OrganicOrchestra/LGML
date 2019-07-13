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

#include "PdNodeUI.h"
#include "../../Controllable/Parameter/UI/TriggerBlinkUI.h"
#include "../UI/ConnectableNodeUI.h"
#include "../../Controllable/Parameter/UI/ParameterUIFactory.h"


PdNodeContentUI::PdNodeContentUI():
isDirty (false)
{

}
PdNodeContentUI::~PdNodeContentUI()
{

    if(pdNode){
        pdNode->removeControllableContainerListener(this);

    }

}

void PdNodeContentUI::init()
{

    pdNode = (PdNode*)node.get();
    addAndMakeVisible (midiDeviceChooser);


    activityBlink = ParameterUIFactory::createDefaultUI (pdNode->midiActivityTrigger);
    activityBlink->showLabel = false;
    addAndMakeVisible (activityBlink);

    midiDeviceChooser = ParameterUIFactory::createDefaultUI(pdNode->midiChooser.getDeviceInEnumParameter());
    addAndMakeVisible(midiDeviceChooser);
    jassert(midiDeviceChooser);

    fileChooser = ParameterUIFactory::createDefaultUI(pdNode->pdPath);
    addAndMakeVisible(fileChooser);



    updatePdParameters();
    setDefaultSize (250, 100);

    pdNode->addControllableContainerListener(this);
    //DBG("Set Node and ui -> " << vstNode->midiPortNameParam->stringValue());

}

void PdNodeContentUI::updatePdParameters()
{
    for (auto& p : paramSliders)
    {
        removeChildComponent (p);
    }

    paramSliders.clear();

    int maxParameter = 20;
    int pCount = 0;

    for (auto& p : pdNode->pdParameters)
    {
        ParameterUI * pui = ParameterUIFactory::createDefaultUI(p);

        if(pui){
            paramSliders.add (pui);
            addAndMakeVisible (pui);

            pCount++;
        }

        if (pCount > maxParameter)
        {
            break;
        }
    }

    resized();
}

void PdNodeContentUI::childControllableAdded (ControllableContainer*, Controllable* c) {
    for (auto& p : paramSliders)
    {
        if (p->parameter == c)removeChildComponent (p);
    }

    if (isDirty) return;

    postCommandMessage (0);
    isDirty = true;
};
void PdNodeContentUI::childControllableRemoved (ControllableContainer*, Controllable* c)
{
    for (auto& p : paramSliders)
    {
        if (p->parameter == c)removeChildComponent (p);
    }

    if (isDirty) return;

    postCommandMessage (0);
    isDirty = true;
}
void PdNodeContentUI::controllableContainerAdded (ControllableContainer*, ControllableContainer*) {};
void PdNodeContentUI::controllableContainerRemoved (ControllableContainer*, ControllableContainer*) {};




void PdNodeContentUI::handleCommandMessage (int /*cId*/)
{
    updatePdParameters();
    isDirty = false;
}


void PdNodeContentUI::resized()
{
    Rectangle<int> area = getLocalBounds().reduced (2);
    int headerHeight = 20;
    int estimatedParamH = jmin(4,paramSliders.size())*20;
    bool displayHeader = area.getHeight()> headerHeight+estimatedParamH;

    fileChooser->setVisible(displayHeader);
    activityBlink->setVisible(displayHeader);
    midiDeviceChooser->setVisible(displayHeader);

    if(displayHeader){
        Rectangle<int> midiR = area.removeFromTop (headerHeight);
        fileChooser->setBounds(midiR.removeFromLeft(midiR.getWidth()/2));
        activityBlink->setBounds (midiR.removeFromRight (midiR.getHeight()/4).reduced (2));
        midiDeviceChooser->setBounds (midiR);
        area.removeFromTop (2);
    }

    layoutSliderParameters (area.reduced (2));

}

void PdNodeContentUI::layoutSliderParameters (Rectangle<int> pArea)
{
    if (paramSliders.size() == 0) return;

    int maxLines = 4;

    int numLines = jmin (maxLines, paramSliders.size());
    int numCols = (paramSliders.size() - 1) / maxLines + 1;

    int w = pArea.getWidth() / numCols;
    int h = pArea.getHeight() / numLines;
    int idx = 0;

    for (int i = 0 ; i < numCols ; i ++)
    {
        Rectangle<int> col = pArea.removeFromLeft (w);

        for (int j = 0 ; j < numLines ; j++)
        {
            paramSliders.getUnchecked (idx)->setBounds (col.removeFromTop (h).reduced (1));
            idx++;

            if (idx >= paramSliders.size())
            {
                break;
            }
        }

        if (idx >= paramSliders.size())
        {
            break;
        }
    }
}

//
//void PdNodeContentUI::vstSelected (int modalResult, Component*   originComp)
//{
//    int index = VSTManager::getInstance()->knownPluginList.getIndexChosenByMenu (modalResult);
//
//    if (index >= 0 )
//    {
//        PdNodeContentUI* originVSTNodeUI =  dynamic_cast<PdNodeContentUI*> (originComp);
//
//        if (originVSTNodeUI)
//        {
//            originVSTNodeUI->vstNode->identifierString->setValue (VSTManager::getInstance()->knownPluginList.getType (index)->createIdentifierString());
//            //            originVSTNodeUI->owner->generatePluginFromDescription(VSTManager::getInstance()->knownPluginList.getType (index));
//        }
//    }
//}


//




#endif
