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
#include "../Style.h"
juce_ImplementSingleton (Inspector)

Inspector::Inspector() :
isListening (true)
,currentEditor (nullptr)
{
    setOpaque(true);
    setPaintingIsUnclipped(true);
}

Inspector::~Inspector()
{
    clear();
}

void Inspector::paint(Graphics & g){
    LGMLUIUtils::fillBackground(this,g);
};

void Inspector::shouldListen (bool value)
{
    if (isListening == value) return;

    if (!value){ setCurrentComponent (nullptr);}
    else{inspectCurrentComponent();}

    isListening = value;
}

void Inspector::clear()
{
    setCurrentComponent (nullptr);
}

int Inspector::getNumSelected(){
    return getItemArray().size();
}

void Inspector::selectComponents(Array<WeakReference<InspectableComponent> > & l){
    deselectAll();
    for(auto & c : l){
        addToSelection(c);
    }
}

void Inspector::setCurrentComponent (InspectableComponent* c)
{
    if(!isListening) return;

    // avoid selection from inspector that will get self-deleted
    if(c && isParentOf(c)){return;}

    jassert(MessageManager::getInstance()->currentThreadHasLockedMessageManager());
    if(c){
        inspectCurrentComponent();
    }
    else{
        clearEditor();
    }

}



ParameterContainer* Inspector::getFirstCurrentContainerSelected()
{
    if(getNumSelected())
        if(auto * s= getItemArray()[0].get())
            return s->getRelatedParameterContainer();
    return nullptr;
}
ParameterBase* Inspector::getFirstCurrentParameterSelected()
{
    if(getNumSelected())
        if(auto * s= getItemArray()[0].get())
            return s->getRelatedParameter();
    return nullptr;
}

bool Inspector::deselectContainer(ControllableContainer * cont){
    for(auto & c:getItemArray()){
        if(c->getRelatedParameterContainer()==cont){
            deselect(c);
            return true;
        }
    }
    return false;
}

InspectableComponent * Inspector::getFirstCurrentComponent(){
    if(getNumSelected()){
        return getItemArray()[0].get();
    }
    return nullptr;

}



void Inspector::resized()
{
    if (currentEditor.get() != nullptr) currentEditor->setBounds (getLocalBounds().reduced (5));
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
    if (currentEditor.get() != nullptr)
    {
        removeChildComponent (currentEditor.get());
        currentEditor->clear();
        currentEditor = nullptr;
    }
}

void Inspector::inspectCurrentComponent()
{


    if (currentEditor != nullptr) currentEditor->removeInspectorEditorListener (this);

    if(getNumSelected()==0) {
        clearEditor();
        return;
    }

    if(isShowing() && getFirstCurrentComponent())
        currentEditor = getFirstCurrentComponent()->createEditor();

    if (currentEditor != nullptr) {
        currentEditor->addInspectorEditorListener (this);
        addAndMakeVisible (currentEditor.get());
    }

    getTopLevelComponent()->toFront (true);

    resized();
}

Array<WeakReference<ParameterContainer> >  Inspector::getContainersSelected(){
    Array<WeakReference<ParameterContainer> >  res;
    for(auto & f:getItemArray()){
        if(f.get())
            res.add(f->getRelatedParameterContainer());
    }
    return res;
}


void Inspector::contentSizeChanged (InspectorEditor*)
{
    listeners.call (&InspectorListener::contentSizeChanged, this);
}

void Inspector::controllableContainerRemoved(ControllableContainer * , ControllableContainer * ori ) {
    for(auto & f:getItemArray()){
        if(f.get() && f->getRelatedParameterContainer()==ori){
            deselect(f);
            return;
        }

    }



}
void Inspector::containerWillClear(ControllableContainer * c){
    for(auto & f:getItemArray()){
        if(f.get() && f->getRelatedParameterContainer()==c){
            deselect(f);
            return;
        }

    }
    
}
void Inspector::controllableRemoved (Controllable* c) {
    for(auto & f:getItemArray()){
        if(f.get() && f->getRelatedParameter()==c){
            deselect(f);
            return;
        }

    }
}



void Inspector::itemSelected (WeakReference<InspectableComponent> c){
    if (c.get()){
        c->setVisuallySelected(true);
        if(auto cc = c->getRelatedParameterContainer())
            cc->addControllableContainerListener(this);
    }
    if(getNumSelected()>0){
        setCurrentComponent(getItemArray()[0]);
    }
    else{
        setCurrentComponent(nullptr);
    }
}

void Inspector::itemDeselected (WeakReference<InspectableComponent> c){
    if (c.get()){
        c->setVisuallySelected(false);
        if(auto cc = c->getRelatedParameterContainer())
            cc->removeControllableContainerListener(this);
    }
    if(getNumSelected()>0){
        setCurrentComponent(getItemArray()[0]);
    }
    else{
        setCurrentComponent(nullptr);
    }
}


////////////
// InspectorViewport
///////////

InspectorViewport::InspectorViewport (const String& contentName, Inspector* _inspector) :
ShapeShifterContentComponent (contentName, "See inside :\nDisplays info on selected Object")
,inspector (_inspector)
{
    vp.setViewedComponent (inspector, false);
    vp.setScrollBarsShown (true, false);
    vp.setScrollOnDragEnabled (false);
    contentIsFlexible = false;
    addAndMakeVisible (vp);
    vp.setScrollBarThickness (10);
    setPaintingIsUnclipped(true);
    vp.setPaintingIsUnclipped(true);
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



    if (inspector->currentEditor.get() == nullptr){

        auto labelR=r.withSizeKeepingCentre(jmax(300,getWidth()),r.getHeight());
        infoLabel.setBounds(labelR);
        infoLabel.setVisible(true);
        inspector->setBounds (r);
    }
    else
    {
        infoLabel.setVisible(false);
        int cH = r.getHeight();
        if(auto * ed = inspector->currentEditor.get()){
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
