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



Outliner::Outliner (const String& contentName,ParameterContainer * _root,bool showFilterText) : ShapeShifterContentComponent (contentName,"Search Parameters in here"),
baseRoot(_root),
root(nullptr),
showUserContainer(true)
{
    if(!baseRoot.get()){
        baseRoot = getEngine();
    }


    showHiddenContainers = false;
    treeView.setIndentSize(10);
    setRoot(baseRoot);
    addAndMakeVisible (treeView);
    treeView.getViewport()->setScrollBarThickness (10);
    if(showFilterText){
        filterTextEditor.setTextToShowWhenEmpty ("search", Colours::grey);
        addAndMakeVisible (filterTextEditor);
        filterTextEditor.addListener (this);
        linkToSelected.setTooltip("link viewed to selected node/controller");
        linkToSelected.setButtonText("L");
        addAndMakeVisible(linkToSelected);
        linkToSelected.addListener(this);
        linkToSelected.setClickingTogglesState(true);
    }
    infoLabel.setVisible(false);


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
    ShapeShifterContentComponent::resized();
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
        rootItem->clearSubItems();
    }
    root = p;
    if (root.get()){

        root->addControllableContainerListener(this);
        rootItem = new OutlinerItem (root,true);
        treeView.setRootItem (rootItem);


        rebuildTree();
        restoreCurrentOpenChilds();
        treeView.setRootItemVisible(root->isUserDefined);
        //        resized();
    }

}

void Outliner::rebuildTree()
{
    rootItem->clearSubItems();
    if(root.get()){
        buildTree (rootItem, root.get());
        rootItem->setOpen (true);
    }

}

void Outliner::buildTree (OutlinerItem* parentItem, ParameterContainer* parentContainer, bool shouldFilter)
{
    bool shouldFilterByName = nameFilter.isNotEmpty() && shouldFilter;
    auto childContainers = parentContainer->getContainersOfType<ParameterContainer> (false);

    for (auto& cc : childContainers)
    {
        if(showUserContainer || !cc->isUserDefined){

            OutlinerItem* ccItem = new OutlinerItem (cc,false);
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
            OutlinerItem* cItem = new OutlinerItem (c,false);
            parentItem->addSubItem (cItem);

        }
    }

    // show every thing on text search
    if (nameFilter.isNotEmpty())
    {
        parentItem->setOpen (true);

    }
}

void Outliner::childStructureChanged (ControllableContainer* notif, ControllableContainer* changed,bool isAdded)
{
    if(root.get()){
    jassert(notif == root);

    if (!AsyncUpdater::isUpdatePending())
    {
        if(nameFilter.isNotEmpty()){
        triggerAsyncUpdate();
        }
    }
    }

}


void Outliner::handleAsyncUpdate()
{
    // generic behaviour waiting engine being ready
    if (getEngine())
    {
        if (getEngine()->isLoadingFile )
        {
            triggerAsyncUpdate();
        }
        else
        {
            // our main root has been deleted, try to come back
            if(!root.get()){
                setRoot(baseRoot.get());
            }
            if(root.get()){
                saveCurrentOpenChilds();

                rebuildTree();
                restoreCurrentOpenChilds();
            }
            else{
                rootItem->clearSubItems();
            }
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
            if(auto sel = Inspector::getInstance()->getCurrentContainerSelected())
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
        if(isParentOf(i->getCurrentComponent()))
            return;


        if(auto sel = i->getCurrentContainerSelected())
            setRoot(sel);
    }
};

//////////////////////////
// OUTLINER ITEM
///////////////////////////

OutlinerItem::OutlinerItem (ParameterContainer* _container,bool generateSubTree) :
container (_container), parameter (nullptr), isContainer (true)
{
    container->addControllableContainerListener(this);

    if(generateSubTree){
    for(auto c:container->getContainersOfType<ParameterContainer>(false)){
       if(!c->isHidenInEditor) addSubItem(new OutlinerItem(c,generateSubTree));
    }

    for(auto c:container->getAllParameters(false,true)){
       if(!c->isHidenInEditor && c!=container->nameParam) addSubItem(new OutlinerItem(c,generateSubTree));
    }
    }
}

OutlinerItem::OutlinerItem (Parameter* _parameter,bool generateSubTree) :
container (nullptr), parameter (_parameter), isContainer (false)
{

    if(auto p = parameter->parentContainer){
        p->addControllableContainerListener(this);

    }
    else{
        jassertfalse;
    }
}

OutlinerItem::~OutlinerItem(){
    if(isContainer){
        if(container){
            container->removeControllableContainerListener(this);
        }
        else{
//            jassertfalse;
        }
    }
    else{
        if(auto p = parameter->parentContainer){
            p->removeControllableContainerListener(this);
        }
        else{
//            jassertfalse;
        }
    }
    masterReference.clear();
}

bool OutlinerItem::mightContainSubItems()
{
    return isContainer;
}

Component* OutlinerItem::createItemComponent()
{

    currentDisplayedComponent = new OutlinerItemComponent (this);
    return currentDisplayedComponent;
}

//TODO better outliner name filtering
void OutlinerItem::controllableContainerAdded(ControllableContainer * notif,ControllableContainer * ori){


    if(notif && notif==container){
        WeakReference<OutlinerItem> item (this);
        WeakReference<ControllableContainer> orir (ori);
        MessageManager::callAsync([item,orir](){
            if(item.get() && orir.get())
                item.get()->addSubItem(new OutlinerItem (dynamic_cast<ParameterContainer*>(orir.get()),true));
        });
    }

    else {
        // if we are notified from other than our container
        jassert(!container);
    }
}
void OutlinerItem::controllableContainerRemoved(ControllableContainer * notif,ControllableContainer * ori){
    if(notif && notif==container){
    int i = 0;
    while( i < getNumSubItems()){
        auto item = dynamic_cast<OutlinerItem*>(getSubItem(i));
        if(item->container==ori){
            removeSubItem(i);
        }
        else{
            i++;
        }
    }
    }
    else if (container){
        jassertfalse;
    }

}



void OutlinerItem::controllableAdded (ControllableContainer* notif, Controllable* ori) {
    if(notif && notif==container){
        MessageManager::callAsync([this , ori](){addSubItem(new OutlinerItem (dynamic_cast<Parameter*>(ori),true));});
    }
    else if (container){
        jassertfalse;
    }
}
void OutlinerItem::controllableRemoved (ControllableContainer* notif, Controllable* ori) {
    if(notif && notif==container){
    int i = 0;
    while( i < getNumSubItems()){
        auto item = dynamic_cast<OutlinerItem*>(getSubItem(i));
        if(item->parameter==ori){
            removeSubItem(i);
        }
        else{
            i++;
        }
    }
    }
    else if (container){
        jassertfalse;
    }

}

String OutlinerItem::getUniqueName() const
{
    // avoid empty names
    if (isContainer) {return "/it/" + container.get()->getControlAddress();}
    else            {return "/it/" + parameter.get()->getControlAddress();}

};


void OutlinerItem::itemSelectionChanged (bool isNowSelected){
    if(auto c= static_cast<OutlinerItemComponent*>(currentDisplayedComponent.get())){
        auto* insp = Inspector::getInstance();
        if(insp->getCurrentComponent()!=c){
            if(isNowSelected ){
                insp->setCurrentComponent(c);
            }
            else{
                insp->setCurrentComponent(nullptr);
            }

        }
    }
}
////////////////////////////
// OutlinerItemComponent
////////////////////////


OutlinerItemComponent::OutlinerItemComponent (OutlinerItem* _item) :
InspectableComponent (_item->container),
item (_item),
label ("label", _item->isContainer ? item->container->getNiceName() : item->parameter->niceName),
paramUI (nullptr)

{
    if(!_item->isContainer){
        InspectableComponent::relatedParameter = _item->parameter;
        InspectableComponent::relatedParameterContainer = nullptr;
    }
    setTooltip (item->isContainer ? item->container->getControlAddress() : item->parameter->description + "\nControl Address : " + item->parameter->controlAddress);
    bool isNameEditable = !item->isContainer && item->parameter->isUserDefined;
    if(item->isContainer)
        isNameEditable|=item->container->nameParam->isEditable;
    if(isNameEditable){
        label.setEditable(false,true,false);
        label.addListener(this);
    }
    addAndMakeVisible (&label);
    label.setInterceptsMouseClicks (false, false);
    if(_item->isContainer && _item->container->isUserDefined){
        addUserParamBt = new AddElementButton();
        addAndMakeVisible(addUserParamBt);
        addUserParamBt->addListener(this);
        if(auto p = _item->container->parentContainer){
            if(p->isUserDefined){
                removeMeBt = new RemoveElementButton();
                addAndMakeVisible(removeMeBt);
                removeMeBt->addListener(this);
            }
        }
    }
    if(!_item->isContainer && _item->parameter->isUserDefined){
        removeMeBt = new RemoveElementButton();
        addAndMakeVisible(removeMeBt);
        removeMeBt->addListener(this);
    }
    if (!_item->isContainer )
    {
        paramUI = ParameterUIFactory::createDefaultUI (item->parameter);
        item->parameter->addControllableListener(this);

    }
    else
    {
        paramUI = ParameterUIFactory::createDefaultUI (item->container->nameParam);
        item->container->nameParam->addAsyncCoalescedListener(this);
    }

    addAndMakeVisible (paramUI);
}
void OutlinerItemComponent::resized()
{
    auto r = getLocalBounds();

    if (paramUI)
    {
        const int minParamDisplayWidth = 200;

        paramUI->setVisible(r.getWidth()>minParamDisplayWidth);
        if(paramUI->isVisible()){
        paramUI->setBounds (r.removeFromRight (r.getWidth() / 2).reduced (2));
        }
    }
    if(addUserParamBt){
        addUserParamBt->setBounds(r.removeFromRight(r.getHeight()).reduced(2));
    }
    if(removeMeBt){
        removeMeBt->setBounds(r.removeFromRight(r.getHeight()).reduced(2));
    }
    label.setBounds (r);

}

void OutlinerItemComponent::controllableNameChanged (Controllable* ) {
    if(!item->isContainer)
        label.setText(  item->parameter->niceName,dontSendNotification);
    else
        jassertfalse;
}

void OutlinerItemComponent::newMessage(const Parameter::ParamWithValue &pv){
    if(item->isContainer && pv.parameter==item->container->nameParam)
        label.setText(  item->container->nameParam->stringValue(),dontSendNotification);
    else
        jassertfalse;
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
        if(item->isContainer){
        expandItems(item,!item->isOpen());
        }

    }
    else{
        item->setSelected (true, true);
        if(label.isEditable())
            label.showEditor();
//        if(item->isContainer)

    }
}



void OutlinerItemComponent::buttonClicked (Button* b){
    if(b==addUserParamBt){
        if(item->isContainer){
            item->container->addNewParameter<FloatParameter> ("variable", "Custom Variable");
        }
        else{
            jassertfalse;
        }
    }
    else if( b==removeMeBt){
        if(item->isContainer){
            item->container->removeFromParent();
        }
        else{
            if(auto p = item->parameter->parentContainer)
                p->removeControllable(item->parameter);
            else jassertfalse;
        }
        
    }
    
}

void OutlinerItemComponent::labelTextChanged (Label* labelThatHasChanged) {
    if(item->isContainer){
        item->container->nameParam->setValue(label.getTextValue().toString());
    }
    else{
        item->parameter->setNiceName(label.getTextValue().toString());
    }
};
