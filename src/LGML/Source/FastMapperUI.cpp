/*
 ==============================================================================

 FastMapperUI.cpp
 Created: 17 May 2016 6:05:39pm
 Author:  bkupe

 ==============================================================================
 */

#include "FastMapperUI.h"
#include "FastMapper.h"
#include "ParameterUIFactory.h"

FastMapperUI::FastMapperUI(FastMapper * _fastMapper, ControllableContainer * _viewFilterContainer) :
fastMapper(_fastMapper), viewFilterContainer(_viewFilterContainer)
{
  fastMapper->addControllableContainerListener(this);

  linkToSelection.setButtonText("Show from selected");
  linkToSelection.setClickingTogglesState(true);
  linkToSelection.addListener(this);
  addAndMakeVisible(linkToSelection);

  autoAddBt = ParameterUIFactory::createDefaultUI(fastMapper->autoAdd);
  addAndMakeVisible(autoAddBt);
  
  addAndMakeVisible(autoAddBt);

  addFastMapBt.setButtonText("Add FastMap");
  addFastMapBt.addListener(this);
  addAndMakeVisible(addFastMapBt);


  potentialIn = new NamedControllableUI(ParameterUIFactory::createDefaultUI(fastMapper->potentialIn),150);
  potentialOut= new NamedControllableUI(ParameterUIFactory::createDefaultUI(fastMapper->potentialOut),150);
  addAndMakeVisible(potentialIn);
  addAndMakeVisible(potentialOut);
  resetAndUpdateView();


}

FastMapperUI::~FastMapperUI()
{
  clear();
  

}

void FastMapperUI::addFastMapUI(FastMap * f)
{
  FastMapUI * fui = new FastMapUI(f);
  mapsUI.add(fui);
  addAndMakeVisible(fui);
  fastMapperUIListeners.call(&FastMapperUIListener::fastMapperContentChanged,this);
}

void FastMapperUI::removeFastMapUI(FastMapUI * fui)
{

  if (fui == nullptr) return;

  removeChildComponent(fui);
  mapsUI.removeObject(fui);

  fastMapperUIListeners.call(&FastMapperUIListener::fastMapperContentChanged, this);
}


void FastMapperUI::resetAndUpdateView()
{

  clear();
  for (auto &f : fastMapper->maps)
  {
    if (mapPassViewFilter(f)) addFastMapUI(f);
  }
  
  resized();
}

void FastMapperUI::setViewFilter(ControllableContainer * filterContainer)
{
  viewFilterContainer = filterContainer;
  resetAndUpdateView();
}

bool FastMapperUI::mapPassViewFilter(FastMap * f)
{

  if (viewFilterContainer == nullptr ) return true;
  if (f->referenceIn->linkedParam != nullptr && (ControllableContainer *)f->referenceIn->linkedParam->isChildOf(viewFilterContainer)) return true;
  if (f->referenceOut->linkedParam != nullptr && (ControllableContainer *)f->referenceOut->linkedParam->isChildOf(viewFilterContainer)) return true;

  return (!f->referenceOut->linkedParam && !f->referenceIn->linkedParam);
}



FastMapUI * FastMapperUI::getUIForFastMap(FastMap *f)
{
  for (auto &fui : mapsUI)
  {
    if (fui->fastMap == f) return fui;
  }

  return nullptr;
}

constexpr int buttonHeight = 21;
int FastMapperUI::getContentHeight()
{

  return mapsUI.size() * (mapHeight + gap) + 5*buttonHeight+10;
}

void FastMapperUI::resized()
{
  Rectangle<int> r = getLocalBounds().reduced(2);
  potentialIn->setBounds(r.removeFromTop(buttonHeight).reduced(2));
  potentialOut->setBounds(r.removeFromTop(buttonHeight).reduced(2));
  autoAddBt->setBounds(r.removeFromTop(buttonHeight).reduced(2));
  linkToSelection.setBounds(r.removeFromTop(buttonHeight).reduced(2));
  addFastMapBt.setBounds(r.removeFromTop(buttonHeight).reduced(2));

  r.removeFromTop(10);
  r.reduce(2, 0);
  for (auto & fui : mapsUI)
  {
    fui->setBounds(r.removeFromTop(mapHeight));
    r.removeFromTop(gap);
  }
}

void FastMapperUI::clear()
{
  while (mapsUI.size() > 0)
  {
    removeFastMapUI(mapsUI[0]);
  }
}

void FastMapperUI::mouseDown(const MouseEvent & e)
{
  if (e.mods.isRightButtonDown())
  {
    PopupMenu m;
    m.addItem(1, "Add Fast Map");
    int result = m.show();
    switch (result)
    {
      case 1:
        fastMapper->addFastMap();
        break;
    }
  }
}


void FastMapperUI::controllableContainerAdded(ControllableContainer* ori,ControllableContainer * cc)
{
  if(ori==fastMapper){

    WeakReference<ControllableContainer> wf(cc);
    MessageManager::callAsync([this,wf] (){
      if(wf.get()){
        addFastMapUI((FastMap*)wf.get());
        resized();}
    });
  }
  //	addFastMapUI(f);
  //	resized();
}

void FastMapperUI::controllableContainerRemoved(ControllableContainer*ori,ControllableContainer *cc)
{
  if(ori==fastMapper){

    WeakReference<Component> fui (getUIForFastMap((FastMap*)cc));
    if(fui.get()){
      if(MessageManager::getInstance()->isThisTheMessageThread()){
        removeFastMapUI(dynamic_cast<FastMapUI*>(fui.get()));
        resized();
      }
      else{
        MessageManager::callAsync([this,fui] (){
          if(fui.get()){
            removeFastMapUI(dynamic_cast<FastMapUI*>(fui.get()));
            resized();
          }
        });
      }
    }
  }
}

void FastMapperUI::buttonClicked (Button* b){
  if(b==&linkToSelection){
    if(linkToSelection.getToggleState()){
      Inspector::getInstance()->addInspectorListener(this);
      setViewFilter(Inspector::getInstance()->getCurrentSelected());
    }
    else{
      Inspector::getInstance()->removeInspectorListener(this);
      setViewFilter(nullptr);
    }
  }

  else if(b==&addFastMapBt ){
    fastMapper->addFastMap();
  }


}

void FastMapperUI::currentComponentChanged(Inspector * i) {
  jassert(linkToSelection.getToggleState());
  if(dynamic_cast<FastMap*>(i->getCurrentSelected())){return;}
  setViewFilter(i->getCurrentSelected());

  
};
