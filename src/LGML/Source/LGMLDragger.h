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



class LGMLDragger : MouseListener{
public:

  juce_DeclareSingleton(LGMLDragger, true);
  void setMainComponent(Component * c,TooltipWindow * tip);

  Component * mainComp;
  TooltipWindow * tip;

  void registerForDrag(LGMLComponent * c);
  void unRegisterForDrag(LGMLComponent * c);

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
      isDragging = false;
      if(!contains(e.getEventRelativeTo(this).getPosition())){
        originComp->repaint();
        LGMLDragger::getInstance()->endDraggingComponent(this,e);
      }
//      else{originComp->mouseExit(e);}

    }
    void mouseUp(const MouseEvent &e)override{
      isDragging = false;
        originComp->repaint();
        LGMLDragger::getInstance()->endDraggingComponent(this,e);
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
