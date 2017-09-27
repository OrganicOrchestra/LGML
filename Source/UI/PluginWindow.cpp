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

#include "PluginWindow.h"


#include "../Node/Impl/VSTNode.h"

class PluginWindow;
static Array <PluginWindow*> activePluginWindows;

PluginWindow::PluginWindow (Component* const pluginEditor,
                            VSTNode* const o,
                            WindowFormatType t)
    : DocumentWindow (pluginEditor->getName(), Colours::grey,
                      DocumentWindow::minimiseButton | DocumentWindow::closeButton),
      owner (o),
      type (t)
{
    setSize (400, 300);

    setContentOwned (pluginEditor, true);


    setTopLeftPosition ((int)owner->pluginWindowParameter.x->value,
                        (int)owner->pluginWindowParameter.y->value);

    owner->pluginWindowParameter.isDisplayed->setValue (true);

    setVisible (true);

    activePluginWindows.add (this);
}

PluginWindow::~PluginWindow()
{
    activePluginWindows.removeFirstMatchingValue (this);
    clearContentComponent();
}


void PluginWindow::closeCurrentlyOpenWindowsFor (VSTNode* p)
{
    for (int i = activePluginWindows.size(); --i >= 0;)
        if (activePluginWindows.getUnchecked (i)->owner == p)
            delete activePluginWindows.getUnchecked (i);
}

void PluginWindow::closeAllCurrentlyOpenWindows()
{
    if (activePluginWindows.size() > 0)
    {
        for (int i = activePluginWindows.size(); --i >= 0;)
            delete activePluginWindows.getUnchecked (i);

        Component dummyModalComp;
        dummyModalComp.enterModalState();
        MessageManager::getInstance()->runDispatchLoopUntil (50);
    }
}
//==============================================================================
PluginWindow* PluginWindow::getWindowFor (VSTNode* const node,
                                          WindowFormatType type)
{
    jassert (node != nullptr);

    for (int i = activePluginWindows.size(); --i >= 0;)
        if (activePluginWindows.getUnchecked (i)->owner == node
            && activePluginWindows.getUnchecked (i)->type == type)
            return activePluginWindows.getUnchecked (i);

    AudioProcessor* processor = dynamic_cast<VSTNode*> (node)->innerPlugin;

    if (!processor)return nullptr;


    AudioProcessorEditor* ui = nullptr;

    if (type == Normal)
    {
        ui = processor->createEditorIfNeeded();

        if (ui == nullptr)
            type = Generic;
    }

    if (ui == nullptr)
    {
        if (type == Generic || type == Parameters)
            ui = new GenericAudioProcessorEditor (processor);

        //        else if (type == Programs)
        //            ui = new ProgramAudioProcessorEditor (processor);
    }

    if (ui != nullptr)
    {
        if (AudioPluginInstance* const plugin = dynamic_cast<AudioPluginInstance*> (processor))
            ui->setName (plugin->getName());

        return new PluginWindow (ui, node, type);
    }

    return nullptr;
}



void PluginWindow::moved()
{
    owner->pluginWindowParameter.x->setValue ((float)getX());
    owner->pluginWindowParameter.y->setValue ((float)getY());
}

void PluginWindow::closeButtonPressed()
{
    owner->pluginWindowParameter.isDisplayed->setValue (false);
    delete this;
}
