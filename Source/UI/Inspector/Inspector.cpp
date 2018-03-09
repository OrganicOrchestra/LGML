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

    // avoid selection from inspector that will get self-deleted
    if(isParentOf(c)){return;}

    if (currentComponent != nullptr || c==nullptr)
    {
        clearEditor();
        if(auto cont = getCurrentContainerSelected())
            cont->removeControllableContainerListener(this);
        currentComponent->setVisuallySelected (false);
        

    }

    currentComponent = c;

    if (currentComponent != nullptr)
    {
        currentComponent->setVisuallySelected (true);
        if(auto cont = getCurrentContainerSelected())
            cont->addControllableContainerListener(this);
    }
    inspectCurrentComponent();
    listeners.call (&InspectorListener::currentComponentChanged, this);
}

ParameterContainer* Inspector::getCurrentContainerSelected()
{
    return currentComponent?currentComponent->getRelatedParameterContainer():nullptr;
}
Parameter* Inspector::getCurrentParameterSelected()
{
    return currentComponent?currentComponent->getRelatedParameter():nullptr;
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

void Inspector::parentHierarchyChanged(){
    if(isShowing()){
        inspectCurrentComponent();
    }
    else{
        currentEditor = nullptr;
    }
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

    if(isShowing())
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
void Inspector::controllableRemoved (Controllable* c) {
    if(c==getCurrentParameterSelected()){
        setCurrentComponent(nullptr);
    }
}

////////////
// InspectorViewport
///////////

InspectorViewport::InspectorViewport (const String& contentName, Inspector* _inspector) :
inspector (_inspector),
ShapeShifterContentComponent (contentName, "See inside :\nDisplays info on selected Object")
{
    vp.setViewedComponent (inspector, false);
    vp.setScrollBarsShown (true, false);
    vp.setScrollOnDragEnabled (false);
    contentIsFlexible = false;
    addAndMakeVisible (vp);
    vp.setScrollBarThickness (10);

    inspector->addInspectorListener (this);

}

InspectorViewport::~InspectorViewport()
{

    inspector->removeInspectorListener(this);
}

void InspectorViewport::resized() 
{
    ShapeShifterContentComponent::resized();
    Rectangle<int> r = getLocalBounds();

    vp.setBounds (r);



    if (inspector->getCurrentEditor() == nullptr){

        auto labelR=r.withSizeKeepingCentre(jmax(300,getWidth()),r.getHeight());
        infoLabel.setBounds(labelR);
        infoLabel.setVisible(true);
        inspector->setBounds (r);
    }
    else
    {
        infoLabel.setVisible(false);
        int cH = r.getHeight();
        if(auto ed = inspector->getCurrentEditor()){
            if(int tH = ed->getContentHeight())
                cH = tH;
        }

        auto tb =r.withPosition (inspector->getPosition()).withHeight (cH);
        if(tb.getHeight()>getHeight())
            tb.removeFromRight (vp.getScrollBarThickness());
        inspector->setBounds (tb);

    }
}

#endif
