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
  DraggedComponent(LGMLComponent * c):originComp(c){
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


  LGMLComponent * originComp;
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
    //      else{originComp->mouseExit(e);}

  }
  void mouseUp(const MouseEvent &e)override{
    if(isDragging){
      LGMLDragger::getInstance()->endDraggingComponent(this,e);
    }
    isDragging = false;
    originComp->repaint();

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
  target = nullptr;
  mainComp = c;
  tip = _tip;
  mainComp->addMouseListener(this, true);
  setMappingActive(false);
}



void LGMLDragger::registerForDrag(LGMLComponent * c){
    unRegisterForDrag(nullptr);

  dragged = new DraggedComponent(c);
//    tip->setMillisecondsBeforeTipAppears(99999999);
  mainComp->addAndMakeVisible(dragged);
  dragged->toFront(false);

}

void LGMLDragger::unRegisterForDrag(LGMLComponent * c){

  jassert(dragged==nullptr || c==nullptr || c==dragged->originComp);
  if(target){
    target->setAlpha(1);
  }
  dragged = nullptr;
  target=nullptr;

  tip->setMillisecondsBeforeTipAppears();


}

void setAllComponentMappingState(Component * c,bool b){
  for(int i = 0 ; i < c->getNumChildComponents() ; i++){
    Component *  ch = c->getChildComponent(i);
    if(ch->isVisible()){
      if(auto lch = dynamic_cast<LGMLComponent*>(ch)){
        lch->setMappingState(b?(lch->isMappingDest?LGMLComponent::MAPDEST:LGMLComponent::MAPSOURCE):LGMLComponent::NOMAP);

      }

        setAllComponentMappingState(ch, b);
      
    }
  }
}
void LGMLDragger::setMappingActive(bool b){
  isMappingActive = b;
  setAllComponentMappingState(mainComp, b);
  if(!b){
    unRegisterForDrag(nullptr);
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
    auto curTarget = dynamic_cast<LGMLComponent*> (curComp);
    if(curTarget!=target && (!curTarget ||curTarget->isMappingDest)){
      if(target){target->setAlpha(1);}
      target = curTarget;
      if(target){
        DBG(curTarget->getName());
        target->setAlpha(0.5);
      }
    }
  }
}
void LGMLDragger::endDraggingComponent(Component *  componentToDrag,const MouseEvent & e){
//  jassert(!target || componentToDrag==target);
  auto target_C = dynamic_cast<ParameterProxyUI*>(target);
  jassert(!target || target_C);
  if(target){
    target_C->paramProxy->setParamToReferTo(dragged->originComp->controllable->getParameter());
  }
  unRegisterForDrag(nullptr);
}


