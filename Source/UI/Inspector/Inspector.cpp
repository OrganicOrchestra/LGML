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


#include "Inspector.h"
#include "../../Controllable/Parameter/ParameterContainer.h"
juce_ImplementSingleton (Inspector)

Inspector::Inspector() :
    currentEditor (nullptr),
    currentComponent (nullptr),
    isEnabled (true)
{
}

Inspector::~Inspector()
{
    clear();
}

void Inspector::setEnabled (bool value)
{
    if (isEnabled == value) return;

    if (!value) setCurrentComponent (nullptr);

    isEnabled = value;
}

void Inspector::clear()
{
    setCurrentComponent (nullptr);
}

void Inspector::setCurrentComponent (InspectableComponent* c)
{
    jassert(MessageManager::getInstance()->currentThreadHasLockedMessageManager());
    if (c == currentComponent) return;

    if (!isEnabled) return;

    if (currentComponent != nullptr || c==nullptr)
    {
        clearEditor();
        if(auto cont = getCurrentContainerSelected())
            cont->removeControllableContainerListener(this);
        currentComponent->setSelected (false);
        

    }

    currentComponent = c;

    if (currentComponent != nullptr)
    {
        currentComponent->setSelected (true);
        if(auto cont = getCurrentContainerSelected())
            cont->addControllableContainerListener(this);
    }
    inspectCurrentComponent();
    listeners.call (&InspectorListener::currentComponentChanged, this);
}

ParameterContainer* Inspector::getCurrentContainerSelected()
{
    if (currentComponent)
    {
        return currentComponent->getRelatedParameterContainer();
    }
    else return nullptr;
}


InspectableComponent * Inspector::getCurrentComponent(){
    if(currentComponent){
        return currentComponent;
    }
    return nullptr;

}

const  InspectorEditor * const Inspector::getCurrentEditor(){
    if(currentEditor){
        return currentEditor;
    }
    return nullptr;

}

void Inspector::resized()
{
    if (currentEditor != nullptr) currentEditor->setBounds (getLocalBounds().reduced (5));
}

void Inspector::clearEditor()
{
    if (currentEditor != nullptr)
    {
        removeChildComponent (currentEditor);
        currentEditor->clear();
        currentEditor = nullptr;
    }
}

void Inspector::inspectCurrentComponent()
{


    if (currentEditor != nullptr) currentEditor->removeInspectorEditorListener (this);

    if (currentComponent == nullptr) return;

    currentEditor = currentComponent->createEditor();

    if (currentEditor != nullptr) currentEditor->addInspectorEditorListener (this);

    addAndMakeVisible (currentEditor);

    getTopLevelComponent()->toFront (true);

    resized();
}



void Inspector::contentSizeChanged (InspectorEditor*)
{
    listeners.call (&InspectorListener::contentSizeChanged, this);
}

void Inspector::controllableContainerRemoved(ControllableContainer * , ControllableContainer * ori ) {
    if(ori== getCurrentContainerSelected()){
        setCurrentComponent(nullptr);
    }


}
void Inspector::containerWillClear(ControllableContainer * ){
    setCurrentComponent(nullptr);
    
}
