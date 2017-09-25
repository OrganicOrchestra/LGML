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


#include "Outliner.h"
#include "../Engine.h"
#include "Style.h"
#include "../Controllable/Parameter/UI/ParameterUIFactory.h"
//#include "../Controllable/UI/ParameterEditor.h"



Outliner::Outliner(const String &contentName) : ShapeShifterContentComponent(contentName)
{
  getEngine()->addControllableContainerListener(this);

  showHiddenContainers = false;

  rootItem = new OutlinerItem(getEngine());
  treeView.setRootItem(rootItem);
  addAndMakeVisible(treeView);
  treeView.getViewport()->setScrollBarThickness(10);
  filterTextEditor.setTextToShowWhenEmpty("search", Colours::grey);
  addAndMakeVisible(filterTextEditor);
  filterTextEditor.addListener(this);
  rebuildTree();
}

Outliner::~Outliner()
{
  if(getEngine()) getEngine()->removeControllableContainerListener(this);

}

void Outliner::resized()
{
  Rectangle<int> r = getLocalBounds();
  r.removeFromTop(20);
  filterTextEditor.setBounds(r.removeFromTop(30));
  treeView.setBounds(r);
}

void Outliner::paint(Graphics & g)
{
  //g.fillAll(Colours::green.withAlpha(.2f));
}


void Outliner::rebuildTree()
{
  rootItem->clearSubItems();
  buildTree(rootItem, getEngine());
  rootItem->setOpen(true);

}

void Outliner::buildTree(OutlinerItem * parentItem, ParameterContainer * parentContainer,bool shouldFilter)
{
  bool shouldFilterByName = nameFilter.isNotEmpty() && shouldFilter;
  auto childContainers = parentContainer->getContainersOfType<ParameterContainer>(false);
  for (auto &cc : childContainers)
  {

    if (cc->skipControllableNameInAddress && !showHiddenContainers)
    {
      buildTree(parentItem, cc,shouldFilter);
    } else
    {
      OutlinerItem * ccItem = new OutlinerItem(cc);
      parentItem->addSubItem(ccItem);

      buildTree(ccItem, cc,!cc->getNiceName().toLowerCase().contains(nameFilter));
      if(shouldFilterByName && ccItem->getNumSubItems()==0 &&
         !cc->getNiceName().toLowerCase().contains(nameFilter)){
        parentItem->removeSubItem(ccItem->getIndexInParent());
      }


    }

  }

  auto childControllables = parentContainer->getControllablesOfType<Parameter>(false);

  for (auto &c : childControllables)
  {
    if(c==parentContainer->nameParam || c->hideInEditor) continue;
    if(!shouldFilterByName || c->niceName.toLowerCase().contains(nameFilter)){
      OutlinerItem * cItem = new OutlinerItem(c);
      parentItem->addSubItem(cItem);

    }
  }
  // show every thing on text search
  if(nameFilter.isNotEmpty()){
    parentItem->setOpen(true);

  }
}

void Outliner::childStructureChanged(ControllableContainer * ,ControllableContainer*)
{
  if(!AsyncUpdater::isUpdatePending()){
    triggerAsyncUpdate();
  }

}


void Outliner::handleAsyncUpdate(){
  if(getEngine()){
    if(getEngine()->isLoadingFile ){
      triggerAsyncUpdate();
    }
    else{
      saveCurrentOpenChilds();
      rootItem->clearSubItems();
      rebuildTree();
      restoreCurrentOpenChilds();
    }
  }
}

void Outliner::textEditorTextChanged (TextEditor& t){
  nameFilter = t.getText().toLowerCase();
  rebuildTree();

}


void Outliner::saveCurrentOpenChilds(){
  xmlState = treeView.getOpennessState(true);
}


void Outliner::restoreCurrentOpenChilds(){
  if(xmlState.get()){treeView.restoreOpennessState(*xmlState.get(),true);}
}

//////////////////////////
// OUTLINER ITEM
///////////////////////////

OutlinerItem::OutlinerItem(ParameterContainer * _container) :
container(_container), parameter(nullptr), isContainer(true)
{

}

OutlinerItem::OutlinerItem(Parameter * _parameter) :
container(nullptr), parameter(_parameter), isContainer(false)
{
}


bool OutlinerItem::mightContainSubItems()
{
  return isContainer;
}

Component * OutlinerItem::createItemComponent()
{
  return new OutlinerItemComponent(this);
}

String OutlinerItem::getUniqueName() const{
  // avoid empty names
  if(isContainer) {return "/it/"+container->getControlAddress();}
  else            {return "/it/"+parameter->getControlAddress();}

};

OutlinerItemComponent::OutlinerItemComponent(OutlinerItem * _item) :
InspectableComponent(_item->container),
item(_item),
label("label",_item->isContainer? item->container->getNiceName() : item->parameter->niceName),
paramUI(nullptr)

{

  setTooltip(item->isContainer ? item->container->getControlAddress() : item->parameter->description + "\nControl Address : " + item->parameter->controlAddress);
  addAndMakeVisible(&label);
  label.setInterceptsMouseClicks(false, false);
  if(!_item->isContainer ){
    paramUI = ParameterUIFactory::createDefaultUI(item->parameter);

  }
  else{
    paramUI = ParameterUIFactory::createDefaultUI(item->container->nameParam);
  }
  addAndMakeVisible(paramUI);
}
void OutlinerItemComponent::resized()
{
  auto r = getLocalBounds();
  if(paramUI){
    paramUI->setBounds(r.removeFromRight(r.getWidth()/2).reduced(2));
  }
  label.setBounds(r);

}
void OutlinerItemComponent::paint(Graphics & g)
{
  Rectangle<int> r = getLocalBounds();

  Colour c = item->isContainer ? HIGHLIGHT_COLOR : TEXT_COLOR;


  int labelWidth = label.getFont().getStringWidthFloat(label.getText());

  if (item->isSelected())
  {
    g.setColour(c);
    g.fillRoundedRectangle(r.withSize(labelWidth + 20, r.getHeight()).toFloat(), 2);
  }

  r.removeFromLeft(3);
  label.setBounds(r);
  label.setColour(Label::textColourId, item->isSelected() ? Colours::grey.darker() : c);



}

void OutlinerItemComponent::mouseDown(const MouseEvent & e)
{
  item->setSelected(true, true);
  selectThis();
}

InspectorEditor * OutlinerItemComponent::createEditor()
{
  if (item->isContainer) return InspectableComponent::createEditor();
  return nullptr;//new ParameterEditor(this,item->parameter);
}
