/*
  ==============================================================================

    LGMLComponent.h
    Created: 20 Jul 2017 8:58:32am
    Author:  Martin Hermant

  ==============================================================================
*/

#pragma once
#include "JuceHeader.h"

class ControllableContainer;
class Controllable;

// this class is virtual to allow double inheritance of Component e.g use with inspectable component
// TODO check if we can/want merge
class LGMLComponent : public virtual Component{
public:
  LGMLComponent(bool isDraggable = true);
  
  void setLGMLElement(Controllable * c);
  void setLGMLElement(ControllableContainer * c);
  void clearElement();
  enum MappingState{
    NOMAP,
    MAPSOURCE,
    MAPDEST
  };
  MappingState mappingState;
  bool isMappingDest;
  void setMappingState(const MappingState  s);
  
  enum LGMLType {
    NONE,
    CONTROLLABLE,
    CONTAINER
  };

  LGMLType type;
  bool isDraggable;
  ControllableContainer * container;
  Controllable * controllable;
  virtual void mouseEnter(const MouseEvent &e)override;
  virtual void mouseExit(const MouseEvent &e)override;
  virtual void paintOverChildren(Graphics &g) override;
  

};
