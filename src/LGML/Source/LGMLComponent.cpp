/*
 ==============================================================================

 LGMLComponent.cpp
 Created: 20 Jul 2017 8:58:32am
 Author:  Martin Hermant

 ==============================================================================
 */

#include "LGMLComponent.h"

#include "ControllableContainer.h"
#include "LGMLDragger.h"



LGMLComponent::LGMLComponent(bool _isDraggable,bool _isMappingDest):isDraggable(_isDraggable),isMappingDest(_isMappingDest){
  clearElement();

  if(LGMLDragger::getInstance()->isMappingActive){
    mappingState = isMappingDest?MAPDEST:MAPSOURCE;
  }
  else{
    mappingState=NOMAP;
  }
}

LGMLComponent::~LGMLComponent(){
  LGMLDragger::getInstance()->unRegisterDragCandidate(this);
}

void LGMLComponent::setMappingDest(bool _isMappingDest){
  isMappingDest = _isMappingDest;
  if(mappingState!=NOMAP){
    mappingState = isMappingDest?MAPDEST:MAPSOURCE;
  }

}

void LGMLComponent::setLGMLElement(Controllable * c){
  clearElement();
  controllable = c;
  setName( c->shortName);
  type = CONTROLLABLE;
}
void LGMLComponent::setLGMLElement(ControllableContainer * c){
  clearElement();
  container = c;
  setName( c->shortName);
  type = CONTAINER;
}

void LGMLComponent::clearElement(){
  controllable = nullptr;
  container = nullptr;
  type = NONE;
}

void  LGMLComponent::setMappingState(const bool  b){
  MappingState s = b?(isMappingDest?MAPDEST:MAPSOURCE):NOMAP;
  if(s!=mappingState){
    if(s==NOMAP){setInterceptsMouseClicks(true, true);}
    else{setInterceptsMouseClicks(true, false);}
  }
  mappingState = s;
  if(mappingState==MAPSOURCE){
    jassert(!isMappingDest);
  }
  if(mappingState==MAPDEST){
    jassert(isMappingDest);
  }
  repaint();
}
void LGMLComponent::paintOverChildren(Graphics &g) {
  jassert(controllable!=nullptr || container!=nullptr);
  Component::paintOverChildren(g);
  if(mappingState!=NOMAP ){
    if(mappingState==MAPSOURCE){
      jassert(!isMappingDest);
      g.setColour(Colours::white.withAlpha(0.5f));
    }
    else{
      jassert(isMappingDest);
      g.setColour(Colours::red.withAlpha(0.5f));
    }
    g.fillAll();
  }
}

void LGMLComponent::mouseEnter(const MouseEvent &e){
  Component::mouseEnter(e);
  if(!isMappingDest && mappingState==MAPSOURCE && isDraggable){
    LGMLDragger::getInstance()->registerDragCandidate(this);
  }
}

void LGMLComponent::mouseExit(const MouseEvent &e){
  Component::mouseExit(e);
  if(!isMappingDest && mappingState==MAPSOURCE && isDraggable && !contains(e.getEventRelativeTo(this).getPosition())){
    LGMLDragger::getInstance()->unRegisterDragCandidate(this);
    repaint();
  }
};

