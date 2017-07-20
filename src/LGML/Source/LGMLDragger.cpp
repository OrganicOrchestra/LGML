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



void LGMLDragger::setMainComponent(Component * c,TooltipWindow * _tip){
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
//  dragged->setAlwaysOnTop(true);

}

void setAllComponentMappingState(Component * c,bool b){
  for(int i = 0 ; i < c->getNumChildComponents() ; i++){
    Component *  ch = c->getChildComponent(i);
    if(ch->isVisible()){
      if(auto lch = dynamic_cast<LGMLComponent*>(ch)){
        lch->setIsMapping(b);
      }
      else{
        setAllComponentMappingState(ch, b);
      }
    }
  }
}
void LGMLDragger::setMappingActive(bool b){
  isMappingActive = b;
  setAllComponentMappingState(mainComp, b);

}
void LGMLDragger::toggleMappingMode(){
  setMappingActive(!isMappingActive);
}


void LGMLDragger::unRegisterForDrag(LGMLComponent * c){
  mainComp->removeChildComponent(dragged.release());
  jassert(dragged==nullptr || c==nullptr || c==dragged->originComp);


    tip->setMillisecondsBeforeTipAppears();


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
  }
}
void LGMLDragger::endDraggingComponent(Component *  componentToDrag,const MouseEvent & e){
  if(LGMLComponent* lgmlC = dynamic_cast<LGMLComponent*>(componentToDrag))
    unRegisterForDrag(lgmlC);
  delete componentToDrag;
  
}
