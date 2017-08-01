/*
 ==============================================================================

 ControllableUI.h
 Created: 9 Mar 2016 12:02:16pm
 Author:  bkupe

 ==============================================================================
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
  NamedControllableUI(ControllableUI * ui,int _labelWidth);
  void resized()override;
  void labelTextChanged (Label* labelThatHasChanged) override;
  Label controllableLabel;
  int labelWidth;
  ScopedPointer <ControllableUI > ownedControllableUI;
};


#endif  // CONTROLLABLEUI_H_INCLUDED
