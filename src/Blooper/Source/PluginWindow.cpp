/*
  ==============================================================================

    PluginWindow.cpp
    Created: 24 Mar 2016 6:58:45pm
    Author:  Martin Hermant

  ==============================================================================
*/


#include "PluginWindow.h"


#include "VSTNode.h"

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

    
    setTopLeftPosition (owner->properties.getWithDefault (getLastXProp (type), Random::getSystemRandom().nextInt (500)),
                        owner->properties.getWithDefault (getLastYProp (type), Random::getSystemRandom().nextInt (500)));
    
    owner->properties.set (getOpenProp (type), true);
    
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
        if (activePluginWindows.getUnchecked(i)->owner == p)
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
        if (activePluginWindows.getUnchecked(i)->owner == node
            && activePluginWindows.getUnchecked(i)->type == type)
            return activePluginWindows.getUnchecked(i);
    
    AudioProcessor* processor = dynamic_cast<VSTNode::VSTProcessor*>(node->audioProcessor)->innerPlugin;
    if(!processor)return nullptr;
    
    
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
    owner->properties.set (getLastXProp (type), getX());
    owner->properties.set (getLastYProp (type), getY());
}

void PluginWindow::closeButtonPressed()
{
    owner->properties.set (getOpenProp (type), false);
    delete this;
}