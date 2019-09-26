/*
 ==============================================================================

 Copyright © Organic Orchestra, 2017

 This file is part of LGML. LGML is a software to manipulate sound in real-time

 This program is free software; you can redistribute it and/or modify
 it under the terms of the GNU General Public License as published by
 the Free Software Foundation (version 3 of the License).

 This program is distributed in the hope that it will be useful,
 but WITHOUT ANY WARRANTY; without even the implied warranty of
 MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.

 ==============================================================================
 */

#if !ENGINE_HEADLESS

#include "Outliner.h"
#include "../Engine.h" // baseRoot and postponing updates
#include "Style.h"
#include "../Controllable/Parameter/UI/ParameterUIFactory.h"
//#include "../Controllable/UI/ParameterEditor.h"

Identifier Outliner::blockSelectionPropagationId("blockSelectionPropagation");

Outliner::Outliner (const String& contentName,ParameterContainer * _root,bool showFilterText) : ShapeShifterContentComponent (contentName,"Search Parameters in here"),
baseRoot(_root),
root(nullptr)
,showOnlyUserContainers(false)
{
    if(!baseRoot.get()){
        baseRoot = getEngine();
    }


    
    treeView.setIndentSize(10);
    setRoot(baseRoot);
    addAndMakeVisible (treeView);
    treeView.getViewport()->setScrollBarThickness (10);
    if(showFilterText){
        filterTextEditor.setComponentID("search");
        filterTextEditor.setTextToShowWhenEmpty ("search", Colours::grey);
        addAndMakeVisible (filterTextEditor);
        filterTextEditor.addListener (this);
        linkToSelected.setTooltip(juce::translate("link viewed to selected node/controller"));
        linkToSelected.setButtonText("L");
        addAndMakeVisible(linkToSelected);
        linkToSelected.addListener(this);
        linkToSelected.setClickingTogglesState(true);
        linkToSelected.setToggleState(true,sendNotification);
        addAndMakeVisible(showOnlyUserContainersB);
        showOnlyUserContainersB.setButtonText("U");
        showOnlyUserContainersB.setTooltip(juce::translate("show only custom user parameters"));
        showOnlyUserContainersB.setClickingTogglesState(true);
        showOnlyUserContainersB.addListener(this);
    }
    infoLabel.setVisible(false);
    

}

Outliner::~Outliner()
{
    setRoot(nullptr,false);
    clear();
    if(auto i = Inspector::getInstanceWithoutCreating())
        i->removeInspectorListener(this);

}

void Outliner::paint(Graphics &g){
    LGMLUIUtils::fillBackground(this,g);

}

void Outliner::clear(){
    setRoot(nullptr);
    opennessStates.clear();

}
void Outliner::resized()
{
    ShapeShifterContentComponent::resized();
    Rectangle<int> r = getLocalBounds();

    if(filterTextEditor.isVisible()){
        r.removeFromTop(10);
        auto headerArea = r.removeFromTop(30);
        linkToSelected.setBounds(headerArea.removeFromLeft(headerArea.getHeight()));
        showOnlyUserContainersB.setBounds(headerArea.removeFromLeft(headerArea.getHeight()));
        filterTextEditor.setBounds (headerArea);

    }
    treeView.setBounds (r);
}

int Outliner::getTargetHeight(){
    return  (filterTextEditor.isVisible()?(10+30):0) + treeView.getViewport()->getViewedComponent()->getHeight();
}



void Outliner::setRoot(ParameterContainer * p,bool saveOpenness){
    if (root.get()){
        root->removeControllableContainerListener (this);
        if(saveOpenness)saveCurrentOpenChilds();
        rootItem->clearSubItems();
    }
    root = p;
    if (root.get()){

        root->addControllableContainerListener(this);
        auto nRoot = new OutlinerItem (root,true);
        treeView.setRootItem (nRoot); // first let treeView clean its old root
        rootItem.reset(nRoot); // then manage the new one and clear old one


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
        buildTree (rootItem.get(), root.get());
        rootItem->setOpen (true);
    }

}

void Outliner::buildTree (OutlinerItem* parentItem, ParameterContainer* parentContainer, bool shouldFilter)
{
    bool shouldFilterByName = nameFilter.isNotEmpty() && shouldFilter;
    auto childContainers = parentContainer->getContainersOfType<ParameterContainer> (false);

    for (auto& cc : childContainers)
    {
        bool buildSubtree = true;
        if(showOnlyUserContainers){
            if(!cc->isUserDefined){ // check if child has
            auto childsC = cc->getAllControllableContainers(true);
            buildSubtree = false;
            for(auto c:childsC){
                if(c && c->isUserDefined){
                    buildSubtree=true;
                    break;
                }
            }

            }

        }
        if(buildSubtree){
            OutlinerItem* ccItem = new OutlinerItem (cc,false);
            parentItem->addSubItem (ccItem);
            
            buildTree (ccItem, cc, !cc->getNiceName().toLowerCase().contains (nameFilter));

            if ((shouldFilterByName && ccItem->getNumSubItems() == 0 &&
                !cc->getNiceName().toLowerCase().contains (nameFilter)) 


                )
            {
                parentItem->removeSubItem (ccItem->getIndexInParent());
            }


        }

    }

    if(!showOnlyUserContainers || parentContainer->isUserDefined){
        auto childControllables = parentContainer->getControllablesOfType<ParameterBase> (false);

        for (auto& c : childControllables)
        {
            if (c == parentContainer->nameParam || c->isHidenInEditor) continue;
            if(!showOnlyUserContainers || c->isUserDefined){
            if (!shouldFilterByName || c->niceName.toLowerCase().contains (nameFilter))
            {
                OutlinerItem* cItem = new OutlinerItem (c,false);
                parentItem->addSubItem (cItem);

            }
            }
        }

    }
    // show every thing on text search
    if (nameFilter.isNotEmpty() || showOnlyUserContainers)
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
void  Outliner::containerWillClear (ControllableContainer* origin) {
    setRoot(nullptr);
    if (!AsyncUpdater::isUpdatePending())
    {

            triggerAsyncUpdate();
        
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

    opennessStates.set(root, new XmlElementCounted(treeView.getOpennessState (true)));

}


void Outliner::restoreCurrentOpenChilds()
{
    XmlElementCounted::Ptr xmlState;
    if(root && opennessStates.contains(root)){
        xmlState = opennessStates[root];
        opennessStates.remove(root);
    }

    if (xmlState && xmlState->xml) {treeView.restoreOpennessState (*xmlState->xml, true);}

}
void Outliner::buttonClicked(Button *b){
    if(b==&linkToSelected){
        if(linkToSelected.getToggleState()){
            Inspector::getInstance()->addInspectorListener(this);
            if(auto sel = Inspector::getInstance()->getFirstCurrentContainerSelected())
                setRoot(sel);
        }
        else{
            Inspector::getInstance()->removeInspectorListener(this);
            setRoot(baseRoot);
        }
    }
    else if(b==&showOnlyUserContainersB){

        showOnlyUserContainers = showOnlyUserContainersB.getToggleState();
        rebuildTree();


    }
}
void Outliner::selectionChanged (Inspector * i ){
    if(linkToSelected.getToggleState()){
        // ignore child components
        if(isParentOf(i->getFirstCurrentComponent()))
            return;


        if(auto sel = i->getFirstCurrentContainerSelected()){
            treeView.getProperties().set(blockSelectionPropagationId, true);
            setRoot(sel);
            treeView.getProperties().set(blockSelectionPropagationId, false);
        }
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

OutlinerItem::OutlinerItem ( ParameterBase* _parameter,bool generateSubTree) :
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
        if(parameter.get()){
        if(auto p = parameter->parentContainer){
            p->removeControllableContainerListener(this);
        }
        else{
//            jassertfalse;
        }
        }
    }
    OutlinerItem::masterReference.clear();
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
        if(!item->container || item->container==ori){
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



void OutlinerItem::childControllableAdded (ControllableContainer* notif, Controllable* ori) {
    if(notif && notif==container){
        auto safeOri = WeakReference<Controllable>(ori);
        auto outlinerItemParent = WeakReference<OutlinerItem>(this);
        MessageManager::callAsync([outlinerItemParent , safeOri](){
            if(safeOri && outlinerItemParent)
                outlinerItemParent.get()->addSubItem(new OutlinerItem (dynamic_cast <ParameterBase*>(safeOri.get()),true));
        });
    }
    else if (container){
        jassertfalse;
    }
}
void OutlinerItem::childControllableRemoved (ControllableContainer* notif, Controllable* ori) {
    WeakReference<OutlinerItem> bailout(this);

    if(notif && notif==container){
        MessageManager::callAsync([bailout,ori]()mutable{
            if(bailout.get()){
                int i = 0;
                while( i < bailout->getNumSubItems()){
                    auto item = dynamic_cast<OutlinerItem*>(bailout->getSubItem(i));
                    if(!item->parameter || item->parameter==ori){
                        bailout->removeSubItem(i);
                    }
                    else{
                        i++;
                    }
                }
            }
        });

    }
    else if (container){
        jassertfalse;
    }
}

String OutlinerItem::getUniqueName() const
{
    // avoid empty names
    if (isContainer) {return "/it" + (container.get()?container.get()->getControlAddress().toString():"old");}
    else            {return "/it" + (parameter.get()?parameter.get()->getControlAddress().toString():"old");}

};


void OutlinerItem::itemSelectionChanged (bool isNowSelected){
    auto owner = getOwnerView();
    if(owner){
        var *v =owner->getProperties().getVarPointer(Outliner::blockSelectionPropagationId);
        if(v!=nullptr && *v)
            return;
    }
    if(auto c= static_cast<OutlinerItemComponent*>(currentDisplayedComponent.get())){
        auto* insp = Inspector::getInstance();
        if(insp->getFirstCurrentComponent()!=c){
            if(isNowSelected ){
                insp->selectOnly(c);
            }
            else{
                insp->deselectAll();
            }

        }
    }
}
////////////////////////////
// OutlinerItemComponent
////////////////////////


OutlinerItemComponent::OutlinerItemComponent (OutlinerItem* _item) :
InspectableComponent ("OutlinerItem"),
item (_item),
label ("label"),
paramUI (nullptr)

{
    if(_item->isContainer){
        InspectableComponent::setRelatedContainer(_item->container);
    }
    else{
        InspectableComponent::setRelatedParameter(_item->parameter);

    }
    setTooltip (item->isContainer ? item->container->getControlAddress().toString() : juce::translate(item->parameter->description) + "\n"+juce::translate("Control Address")+" : "  + item->parameter->controlAddress.toString());
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
        addUserParamBt = std::make_unique< AddElementButton>();
        addAndMakeVisible(addUserParamBt.get());
        addUserParamBt->addListener(this);
        if(auto p = _item->container->parentContainer){
            if(p->isUserDefined){
                removeMeBt = std::make_unique< RemoveElementButton>();
                addAndMakeVisible(removeMeBt.get());
                removeMeBt->addListener(this);
            }
        }
    }
    if(!_item->isContainer && _item->parameter->isUserDefined){
        removeMeBt = std::make_unique< RemoveElementButton>();
        addAndMakeVisible(removeMeBt.get());
        removeMeBt->addListener(this);
    }
    if (!_item->isContainer )
    {
        paramUI = ParameterUIFactory::createDefaultUI (item->parameter);
        paramUI->showLabel = false;
        item->parameter->addControllableListener(this);



    }
    else
    {
        paramUI = ParameterUIFactory::createDefaultUI (item->container->nameParam);
        item->container->nameParam->addAsyncCoalescedListener(this);
    }
//    label.setPaintingIsUnclipped(true);
    LGMLUIUtils::optionallySetBufferedToImage(&label);
    updateLabelText();

    addAndMakeVisible (paramUI.get());
}

OutlinerItemComponent::~OutlinerItemComponent(){
    if(paramUI && paramUI->parameter.get()){
        paramUI->parameter->removeControllableListener(this);
        paramUI->parameter->removeAsyncParameterListener(this);
    }
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

void OutlinerItemComponent::updateLabelText(){
    String t = "error";
    if(item->isContainer){
        if(item->container) t= item->container->getNiceName();
        else jassertfalse;
    }
    else{
        if(item->parameter) t=item->parameter->niceName;
        else jassertfalse;
    }
//    item->container->nameParam->stringValue()
    label.setText(  t,dontSendNotification);
    labelWidth = label.getFont().getStringWidthFloat (t);
}

void OutlinerItemComponent::controllableNameChanged (Controllable* ) {
    if(!item->isContainer){
        updateLabelText();
    }
    else
        jassertfalse;
}



void OutlinerItemComponent::newMessage(const ParameterBase::ParamWithValue &pv){
    if(item->isContainer && pv.parameter==item->container->nameParam){
        updateLabelText();
    }
    else
        {jassertfalse;}
}
void OutlinerItemComponent::paint (Graphics& g)
{
    LGMLUIUtils::fillBackground(this, g);
    Rectangle<int> r = getLocalBounds();
    Colour c = item->isContainer ? findColour (TextButton::buttonOnColourId) : findColour (Label::textColourId);




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
        m.addItem(1, juce::translate("expand all childs"));
        m.addItem(2, juce::translate("close all childs"));

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
    if(b==addUserParamBt.get()){
        if(item->isContainer){
            item->container->addNewParameter<FloatParameter> ("variable", "Custom Variable");
        }
        else{
            jassertfalse;
        }
    }
    else if( b==removeMeBt.get()){
        if(item->isContainer){
            item->container->removeFromParent();
        }
        else{
            auto p = item->parameter;
            if(p && p->parentContainer){
                    p->parentContainer->removeControllable(p);
            }
            else{
                jassertfalse;

            }
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
#endif
