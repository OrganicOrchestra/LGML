/*
 ==============================================================================

 Copyright © Organic Orchestra, 2017

 This file is part of LGML. LGML is a software to manipulate sound in realtime

 This program is free software; you can redistribute it and/or modify
 it under the terms of the GNU General Public License as published by
 the Free Software Foundation (version 3 of the License).

 This program is distributed in the hope that it will be useful,
 but WITHOUT ANY WARRANTY; without even the implied warranty of
 MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.

 ==============================================================================
 */

#include "Outliner.h"
#include "../Engine.h"
#include "Style.h"
#include "../Controllable/Parameter/UI/ParameterUIFactory.h"
//#include "../Controllable/UI/ParameterEditor.h"



Outliner::Outliner (const String& contentName,ParameterContainer * _root,bool showFilterText) : ShapeShifterContentComponent (contentName),
baseRoot(_root),
root(nullptr)
{
    if(!baseRoot.get()){
        baseRoot = getEngine();
    }


    showHiddenContainers = false;

    setRoot(baseRoot);
    addAndMakeVisible (treeView);
    treeView.getViewport()->setScrollBarThickness (10);
    if(showFilterText){
        filterTextEditor.setTextToShowWhenEmpty ("search", Colours::grey);
        addAndMakeVisible (filterTextEditor);
        filterTextEditor.addListener (this);
        addAndMakeVisible(linkToSelected);
        linkToSelected.addListener(this);
        linkToSelected.setClickingTogglesState(true);
    }

}

Outliner::~Outliner()
{

    clear();
    if(auto i = Inspector::getInstanceWithoutCreating())
        i->removeInspectorListener(this);

}

void Outliner::clear(){
    setRoot(nullptr);
    for(auto  k : opennessStates){
        delete k;
    }
    opennessStates.clear();

}
void Outliner::resized()
{
    Rectangle<int> r = getLocalBounds();
    r.removeFromTop (20);
    if(filterTextEditor.isVisible()){
        auto headerArea = r.removeFromTop(30);
        linkToSelected.setBounds(headerArea.removeFromLeft(headerArea.getHeight()));
        filterTextEditor.setBounds (headerArea);

    }
    treeView.setBounds (r);
}

void Outliner::paint (Graphics& g)
{
    //g.fillAll(Colours::green.withAlpha(.2f));
}


void Outliner::setRoot(ParameterContainer * p){
    if (root.get()){
        root->removeControllableContainerListener (this);
        saveCurrentOpenChilds();
    }
    root = p;
    if (root.get()){

        root->addControllableContainerListener(this);
        rootItem = new OutlinerItem (root);
        treeView.setRootItem (rootItem);

        treeView.setRootItemVisible(false);
        rebuildTree();
        restoreCurrentOpenChilds();
        //        resized();
    }

}

void Outliner::rebuildTree()
{
    rootItem->clearSubItems();
    buildTree (rootItem, root.get());
    rootItem->setOpen (true);

}

void Outliner::buildTree (OutlinerItem* parentItem, ParameterContainer* parentContainer, bool shouldFilter)
{
    bool shouldFilterByName = nameFilter.isNotEmpty() && shouldFilter;
    auto childContainers = parentContainer->getContainersOfType<ParameterContainer> (false);

    for (auto& cc : childContainers)
    {

        if (cc->skipControllableNameInAddress && !showHiddenContainers)
        {
            buildTree (parentItem, cc, shouldFilter);
        }
        else
        {
            OutlinerItem* ccItem = new OutlinerItem (cc);
            parentItem->addSubItem (ccItem);

            buildTree (ccItem, cc, !cc->getNiceName().toLowerCase().contains (nameFilter));

            if (shouldFilterByName && ccItem->getNumSubItems() == 0 &&
                !cc->getNiceName().toLowerCase().contains (nameFilter))
            {
                parentItem->removeSubItem (ccItem->getIndexInParent());
            }


        }

    }

    auto childControllables = parentContainer->getControllablesOfType<Parameter> (false);

    for (auto& c : childControllables)
    {
        if (c == parentContainer->nameParam || c->isHidenInEditor) continue;

        if (!shouldFilterByName || c->niceName.toLowerCase().contains (nameFilter))
        {
            OutlinerItem* cItem = new OutlinerItem (c);
            parentItem->addSubItem (cItem);

        }
    }

    // show every thing on text search
    if (nameFilter.isNotEmpty())
    {
        parentItem->setOpen (true);

    }
}

void Outliner::childStructureChanged (ControllableContainer*, ControllableContainer*)
{
    if (!AsyncUpdater::isUpdatePending())
    {
        triggerAsyncUpdate();
    }

}


void Outliner::handleAsyncUpdate()
{
    // generic behaviour waiting angine being ready
    if (getEngine())
    {
        if (getEngine()->isLoadingFile )
        {
            triggerAsyncUpdate();
        }
        else
        {
            saveCurrentOpenChilds();
            rootItem->clearSubItems();
            rebuildTree();
            restoreCurrentOpenChilds();
        }
    }
}

void Outliner::textEditorTextChanged (TextEditor& t)
{
    nameFilter = t.getText().toLowerCase();
    rebuildTree();

}


void Outliner::saveCurrentOpenChilds()
{
    if(opennessStates.contains(root)){
        delete opennessStates[root];
    }
    opennessStates.set(root, treeView.getOpennessState (true));

}


void Outliner::restoreCurrentOpenChilds()
{
    ScopedPointer <XmlElement> xmlState (nullptr);
    if(root && opennessStates.contains(root)){
        xmlState = opennessStates[root];
        opennessStates.remove(root);
    }

    if (xmlState.get()) {treeView.restoreOpennessState (*xmlState.get(), true);}
    
}
void Outliner::buttonClicked(Button *b){
    if(b==&linkToSelected){
        if(linkToSelected.getToggleState()){
            Inspector::getInstance()->addInspectorListener(this);
            if(auto sel = Inspector::getInstance()->getCurrentSelected())
                setRoot(sel);
        }
        else{
            Inspector::getInstance()->removeInspectorListener(this);
            setRoot(baseRoot);
        }
    }
}
void Outliner::currentComponentChanged (Inspector * i ){
    if(linkToSelected.getToggleState()){
        // ignore child components
        if(isParentOf(i->currentComponent))
            return;


        if(auto sel = i->getCurrentSelected())
            setRoot(sel);
    }
};

//////////////////////////
// OUTLINER ITEM
///////////////////////////

OutlinerItem::OutlinerItem (ParameterContainer* _container) :
container (_container), parameter (nullptr), isContainer (true)
{

}

OutlinerItem::OutlinerItem (Parameter* _parameter) :
container (nullptr), parameter (_parameter), isContainer (false)
{
}


bool OutlinerItem::mightContainSubItems()
{
    return isContainer;
}

Component* OutlinerItem::createItemComponent()
{
    return new OutlinerItemComponent (this);
}

String OutlinerItem::getUniqueName() const
{
    // avoid empty names
    if (isContainer) {return "/it/" + container->getControlAddress();}
    else            {return "/it/" + parameter->getControlAddress();}

};

OutlinerItemComponent::OutlinerItemComponent (OutlinerItem* _item) :
InspectableComponent (_item->container),
item (_item),
label ("label", _item->isContainer ? item->container->getNiceName() : item->parameter->niceName),
paramUI (nullptr)

{

    setTooltip (item->isContainer ? item->container->getControlAddress() : item->parameter->description + "\nControl Address : " + item->parameter->controlAddress);
    addAndMakeVisible (&label);
    label.setInterceptsMouseClicks (false, false);

    if (!_item->isContainer )
    {
        paramUI = ParameterUIFactory::createDefaultUI (item->parameter);

    }
    else
    {
        paramUI = ParameterUIFactory::createDefaultUI (item->container->nameParam);
    }

    addAndMakeVisible (paramUI);
}
void OutlinerItemComponent::resized()
{
    auto r = getLocalBounds();

    if (paramUI)
    {
        paramUI->setBounds (r.removeFromRight (r.getWidth() / 2).reduced (2));
    }

    label.setBounds (r);

}
void OutlinerItemComponent::paint (Graphics& g)
{
    Rectangle<int> r = getLocalBounds();

    Colour c = item->isContainer ? findColour (TextButton::buttonOnColourId) : findColour (Label::textColourId);


    int labelWidth = label.getFont().getStringWidthFloat (label.getText());

    if (item->isSelected())
    {
        g.setColour (c);
        g.fillRoundedRectangle (r.withSize (labelWidth + 20, r.getHeight()).toFloat(), 2);
    }

    r.removeFromLeft (3);
    label.setBounds (r);
    label.setColour (Label::textColourId, item->isSelected() ? Colours::grey.darker() : c);



}


void expandItems(TreeViewItem * c,const bool s){
    c->setOpen(s);
    for (int i = 0 ; i <c->getNumSubItems() ; i++ ){
        expandItems(c->getSubItem(i), s);
    }


}

void OutlinerItemComponent::mouseDown (const MouseEvent& e)
{

    if(e.mods.isRightButtonDown() && item->isContainer){
        PopupMenu m;
        m.addItem(1, "expand all childs");
        m.addItem(2, "close all childs");

        auto res = m.showAt(this);
        if(res==1 || res==2){
            expandItems(item,res==1);
        }
    }
    else if(e.getNumberOfClicks()>=2){
        expandItems(item,!item->isOpen());
    }
    else{
        item->setSelected (true, true);
        if(item->isContainer)
            selectThis();
    }
}

InspectorEditor* OutlinerItemComponent::createEditor()
{
    if (item->isContainer) return InspectableComponent::createEditor();
    
    return nullptr;//new ParameterEditor(this,item->parameter);
}
