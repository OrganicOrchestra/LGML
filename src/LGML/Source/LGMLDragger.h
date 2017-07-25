/*
 ==============================================================================

 ComponentDragger.h
 Created: 20 Jul 2017 8:47:29am
 Author:  Martin Hermant

 ==============================================================================
 */

#pragma once

#include "JuceHeader.h"
#include "LGMLComponent.h"

//DBGinclude
#include "Controllable.h"

class DraggedComponent;
class LGMLDragger : MouseListener{
public:

  LGMLDragger();
  ~LGMLDragger();
  juce_DeclareSingleton(LGMLDragger, true);

  void setMainComponent(Component * c,TooltipWindow * tip);

  Component * mainComp;
  TooltipWindow * tip;

  void registerForDrag(LGMLComponent * c);
  void unRegisterForDrag(LGMLComponent * c);



  void startDraggingComponent (Component* const componentToDrag, const MouseEvent& e);
  void dragComponent (Component* const componentToDrag, const MouseEvent& e,ComponentBoundsConstrainer* const constrainer);
  void endDraggingComponent(Component *  componentToDrag,const MouseEvent & e);



  ScopedPointer<DraggedComponent> dragged;
  void setMappingActive(bool isActive);
  void toggleMappingMode();
  bool isMappingActive;

  Component*  target;
private:
  Point<int> mouseDownWithinTarget;


};

