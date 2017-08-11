/*
 ==============================================================================

 ComponentDragger.cpp
 Created: 20 Jul 2017 8:47:29am
 Author:  Martin Hermant

 ==============================================================================
 */

#include "LGMLDragger.h"
#include "MainComponent.h"

juce_ImplementSingleton(LGMLDragger);

#include "ParameterProxyUI.h"



/////////////////////
// DraggedComponent
///////////////////

class DraggedComponent : public Component{
public:
  DraggedComponent(ControllableUI * c):originComp(c){
    Rectangle<int > bounds = c->getScreenBounds();
    bounds-=LGMLDragger::getInstance()->mainComp->getScreenBounds().getTopLeft();
    draggedImage = c->createComponentSnapshot(c->getLocalBounds());
    setBounds(bounds);
    setOpaque(false);
    setInterceptsMouseClicks(true, false);
    isDragging = false;


  }
  ~DraggedComponent(){
    removeMouseListener(this);
  }


  ControllableUI * originComp;
  Image draggedImage;
  bool isDragging;
  void mouseDrag(const MouseEvent & e)override{
    LGMLDragger::getInstance()->dragComponent(this, e, nullptr);

  }
  void mouseDown(const MouseEvent &e)override{
    isDragging = true;
    LGMLDragger::getInstance()->startDraggingComponent(this, e);
  }

  void mouseExit(const MouseEvent &e)override{
    if(isDragging){
      isDragging = false;
      if(!contains(e.getEventRelativeTo(this).getPosition())){
        originComp->repaint();
        LGMLDragger::getInstance()->endDraggingComponent(this,e);
      }
    }
    LGMLDragger::getInstance()->unRegisterDragCandidate(originComp);
    //      else{originComp->mouseExit(e);}

  }
  void mouseUp(const MouseEvent &e)override{
    if(isDragging){
      LGMLDragger::getInstance()->endDraggingComponent(this,e);
    }
    isDragging = false;
    originComp->repaint();
    LGMLDragger::getInstance()->unRegisterDragCandidate(originComp);

  }
  void paint(Graphics &g) override{
    g.drawImage( draggedImage, getLocalBounds().toFloat());
    g.setColour(Colours::white);
    g.drawFittedText(originComp->getName(), getLocalBounds(), Justification::centred, 2);
  }
  void paintOverChildren(Graphics &g) override{

    g.setColour(Colours::green.withAlpha(0.5f));
    g.fillAll();
  }

  bool hitTest(int x,int y)override{
    return !isDragging;
  }

};




////////////////////
// LGMLDragger
///////////////
LGMLDragger::LGMLDragger(){

}
LGMLDragger::~LGMLDragger(){

}
void LGMLDragger::setMainComponent(Component * c,TooltipWindow * _tip){
  dragCandidate = nullptr;
  dropCandidate=nullptr;
  selected = nullptr;
  mainComp = c;
  tip = _tip;
  mainComp->addMouseListener(this, true);
  setMappingActive(false);
}



void LGMLDragger::registerDragCandidate(ControllableUI * c){
  //    unRegisterForDrag(nullptr);

  dragCandidate = new DraggedComponent(c);
  //    tip->setMillisecondsBeforeTipAppears(99999999);
  mainComp->addAndMakeVisible(dragCandidate);
  dragCandidate->toFront(false);

}

void LGMLDragger::unRegisterDragCandidate(ControllableUI * c){
  dragCandidate = nullptr;
  if(dropCandidate){
    dropCandidate->setAlpha(1);
  }
  dropCandidate=nullptr;
  //  tip->setMillisecondsBeforeTipAppears();


}
ControllableUI * getUIForComp(Component *c){
  Component * insp = c;
  while (insp!=nullptr){
    if(auto vc = dynamic_cast<ControllableUI*>(insp)){
      return vc;
    }
    insp = insp->getParentComponent();
  }
  return nullptr;
}

void LGMLDragger::mouseEnter(const MouseEvent &e){

  if(auto c = getUIForComp(e.originalComponent)){
    if(!c->isMappingDest &&
       c->mappingState==ControllableUI::MAPSOURCE &&
       c->isDraggable){
      registerDragCandidate(c);
    }
  }
}

void LGMLDragger::mouseUp(const MouseEvent &e){
//  unselect only if in the same parent component
  auto * i = e.originalComponent;
  while (i) {
    if(i==selectedSSContent){
      setSelected(nullptr);
    }
    i = i->getParentComponent();
  }


}
void LGMLDragger::mouseExit(const MouseEvent &e){
  if(auto c = getUIForComp(e.originalComponent)){
    if(!c->isMappingDest &&
       c->mappingState==ControllableUI::MAPSOURCE &&
       c->isDraggable &&
       !c->contains(e.getEventRelativeTo(c).getPosition())){
      unRegisterDragCandidate(c);
      c->repaint();
    }
  }
};

void setAllComponentMappingState(Component * c,bool b){
  for(int i = 0 ; i < c->getNumChildComponents() ; i++){
    Component *  ch = c->getChildComponent(i);
    if(ch->isVisible()){
      if(auto lch = dynamic_cast<ControllableUI*>(ch)){
        auto *p = lch->controllable->getParameter();
        if(p->isEditable ){
          lch->setMappingState(b);
        }


      }

      setAllComponentMappingState(ch, b);

    }
  }
}
void LGMLDragger::setMappingActive(bool b){
  isMappingActive = b;
  setAllComponentMappingState(mainComp, b);
  if(!b){
    unRegisterDragCandidate(nullptr);
  }
  else{
    MouseInputSource mainMouse = Desktop::getInstance().getMainMouseSource();
    if(auto c = dynamic_cast<ControllableUI*>(mainMouse.getComponentUnderMouse())){
      registerDragCandidate(c);
    }
  }


}
void LGMLDragger::toggleMappingMode(){
  setMappingActive(!isMappingActive);
}




void LGMLDragger::startDraggingComponent (Component* const componentToDrag, const MouseEvent& e)
{
  jassert (componentToDrag != nullptr);
  jassert (e.mods.isAnyMouseButtonDown()); // The event has to be a drag event!

  if (componentToDrag != nullptr)
    mouseDownWithinTarget = e.getEventRelativeTo (componentToDrag).getMouseDownPosition();


}



void LGMLDragger::dragComponent (Component* const componentToDrag, const MouseEvent& e,
                                 ComponentBoundsConstrainer* const constrainer)
{
  jassert (componentToDrag != nullptr);
  jassert (e.mods.isAnyMouseButtonDown()); // The event has to be a drag event!

  if (componentToDrag != nullptr)
  {
    Rectangle<int> bounds (componentToDrag->getBounds());

    // If the component is a window, multiple mouse events can get queued while it's in the same position,
    // so their coordinates become wrong after the first one moves the window, so in that case, we'll use
    // the current mouse position instead of the one that the event contains...
    if (componentToDrag->isOnDesktop())
      bounds += componentToDrag->getLocalPoint (nullptr, e.source.getScreenPosition()).roundToInt() - mouseDownWithinTarget;
    else
      bounds += e.getEventRelativeTo (componentToDrag).getPosition() - mouseDownWithinTarget;

    if (constrainer != nullptr)
      constrainer->setBoundsForComponent (componentToDrag, bounds, false, false, false, false);
    else
      componentToDrag->setBounds (bounds);
    auto curComp = mainComp->getComponentAt(e.getEventRelativeTo(mainComp).getPosition());
    // juce still return child of component that doesn't allow click on child
    if(curComp)curComp=curComp->getParentComponent();
    auto curTarget = dynamic_cast<ControllableUI*> (curComp);
    if(curTarget!=dropCandidate && (!curTarget ||curTarget->isMappingDest)){
      if(dropCandidate){dropCandidate->setAlpha(1);}
      dropCandidate = curTarget;
      if(dropCandidate){
        DBG(curTarget->getName());
        dropCandidate->setAlpha(0.5);
      }
    }
  }
}
void LGMLDragger::endDraggingComponent(Component *  componentToDrag,const MouseEvent & e){
  //  jassert(!target || componentToDrag==target);
  auto target_C = dynamic_cast<ParameterProxyUI*>(dropCandidate);
  jassert(!dropCandidate || target_C);
  if(dropCandidate){
    target_C->paramProxy->setParamToReferTo(dragCandidate->originComp->controllable->getParameter());
  }
  else{

    auto *c = dragCandidate?dragCandidate->originComp:nullptr;
    setSelected(c);
  }
  unRegisterDragCandidate(nullptr);
}

void LGMLDragger::setSelected(ControllableUI * c){

  if(c){
    Component *i  = c;
    while(i){
      if(dynamic_cast<ShapeShifterContent*>(i)){
        selectedSSContent = i;
      }
      i= i->getParentComponent();
    }
  }
  else{
    selectedSSContent = nullptr;
  }

  if(c!=selected.get()){
    if(selected.get()){
      ((ControllableUI*)selected.get())->isSelected = false;
      selected->repaint();
    }

    selected = c;
    if(selected.get()){
      auto *cUI = ((ControllableUI*)selected.get());
      cUI->isSelected = true;
      selected->repaint();
     
    }
    listeners.call(&Listener::selectionChanged,c? c->controllable:nullptr);

  }
}


