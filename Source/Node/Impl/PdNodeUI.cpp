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
        pdNode->isLoadedParam->removeAsyncParameterListener(this);

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
    updatePdParameters();
    setDefaultSize (250, 100);

    pdNode->addControllableContainerListener(this);
    pdNode->isLoadedParam->addAsyncCoalescedListener(this);
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

void PdNodeContentUI::childControllableAdded (ControllableContainer*, Controllable*) {};
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




void PdNodeContentUI::newMessage (const ParameterBase::ParamWithValue& pv) {

    if(pv.parameter == pdNode->isLoadedParam){
        if (isDirty) return;

        postCommandMessage (0);
        isDirty = true;
    }
}

void PdNodeContentUI::handleCommandMessage (int /*cId*/)
{
    updatePdParameters();
    isDirty = false;
}


void PdNodeContentUI::resized()
{
    Rectangle<int> area = getLocalBounds().reduced (2);
    Rectangle<int> midiR = area.removeFromTop (25);
    activityBlink->setBounds (midiR.removeFromRight (midiR.getHeight()).reduced (2));
    midiDeviceChooser->setBounds (midiR);

    area.removeFromTop (2);
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
//void PdNodeContentUI::buttonClicked (Button* button)
//{
//    if (button == &VSTListShowButton)
//    {
//        PopupMenu  VSTList;
//        VSTManager::getInstance()->knownPluginList.addToMenu (VSTList, KnownPluginList::SortMethod::sortByCategory);
//        closePluginWindow();
//        VSTList.showAt (&VSTListShowButton, 0, 0, 0, 0, ModalCallbackFunction::forComponent (&PdNodeContentUI::vstSelected, (Component*)this));
//
//    }
//
//    if (button == &showPluginWindowButton)
//    {
//        createPluginWindow();
//    }
//}



#endif
