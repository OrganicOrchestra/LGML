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


#ifndef CONTROLLABLEUI_H_INCLUDED
#define CONTROLLABLEUI_H_INCLUDED


#include "Controllable.h"
#include "Style.h"//keep


class ControllableUI : public Component,public SettableTooltipClient , public Controllable::Listener
{
public:
  ControllableUI(Controllable * controllable);
  virtual ~ControllableUI();



  WeakReference<Controllable>  controllable;


  // Inherited via Listener
  virtual void controllableStateChanged(Controllable * c) override;
  virtual void controllableControlAddressChanged(Controllable * c) override;

  enum MappingState{
    NOMAP,
    MAPSOURCE,
    MAPDEST
  };

  void setMappingState(const bool  s);
  void setMappingDest(bool _isMappingDest);

  bool isDraggable;
  bool isSelected;


  virtual void paintOverChildren(Graphics &g) override;
  bool isMappingDest;

private:

  friend class LGMLDragger;
  MappingState mappingState;
  bool hasValidControllable;

  JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(ControllableUI)

protected :
  void updateTooltip();
  void mouseDown(const MouseEvent &e) override;
};


//    this class allow to automaticly generate label / ui element for parameter listing in editor
//    it owns the created component
class NamedControllableUI : public ControllableUI,public Label::Listener
{
public:
  NamedControllableUI(ControllableUI * ui,int _labelWidth,bool labelAbove=false);
  void resized()override;
  bool labelAbove;
  void labelTextChanged (Label* labelThatHasChanged) override;
  Label controllableLabel;
  int labelWidth;
  ScopedPointer <ControllableUI > ownedControllableUI;
};




#endif  // CONTROLLABLEUI_H_INCLUDED
