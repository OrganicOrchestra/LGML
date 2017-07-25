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

// this class is virtual to allow double inheritance of Component e.g use with InspectableComponent in OutlinerComponent
// TODO check if we can/want merge
class LGMLComponent : public virtual Component{
public:
  LGMLComponent(bool isDraggable = true,bool _isMappingDest=false);
  virtual ~LGMLComponent();
  void setLGMLElement(Controllable * c);
  void setLGMLElement(ControllableContainer * c);
  void clearElement();
  enum MappingState{
    NOMAP,
    MAPSOURCE,
    MAPDEST
  };

  void setMappingState(const bool  s);
  void setMappingDest(bool _isMappingDest);
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
  bool isMappingDest;
  
private:
  MappingState mappingState;

};
