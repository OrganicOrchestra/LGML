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


#ifndef CONNECTABLENODEHEADERUI_H_INCLUDED
#define CONNECTABLENODEHEADERUI_H_INCLUDED
#pragma once

#include "StringParameterUI.h"
#include "BoolToggleUI.h"
#include "ConnectableNode.h"
#include "PresetChooser.h"


class VuMeter;
class ConnectableNodeUI;

class ConnectableNodeHeaderUI : public Component,
public Button::Listener,
private ControllableContainerListener,
public ConnectableNode::ConnectableNodeListener
{
public:

  class Grabber : public Component
  {
    void paint(Graphics &g) override;
  };

  ConnectableNodeHeaderUI();
  virtual ~ConnectableNodeHeaderUI();

  ConnectableNode * node;
  ConnectableNodeUI * nodeUI;

  ScopedPointer<StringParameterUI> titleUI;
  ScopedPointer<StringParameterUI> descriptionUI;
  ScopedPointer<ParameterUI> enabledUI;
  ScopedPointer<VuMeter> vuMeterIn;
  ScopedPointer<VuMeter> vuMeterOut;

  Grabber grabber;
  ImageButton removeBT;
  TextButton miniModeBT;
  ScopedPointer<PresetChooser> presetChooser;


  virtual void setNodeAndNodeUI(ConnectableNode * node, ConnectableNodeUI * nodeUI);
  virtual void init();


  virtual void resized() override;

  void updateVuMeters();

  bool bMiniMode;
  virtual void setMiniMode(bool value);

  // Inherited via Listeners
  virtual void nodeParameterChanged(ConnectableNode *,Parameter *) override;

  virtual void buttonClicked(Button *) override;
  virtual void controllableContainerPresetLoaded(ControllableContainer *) override;


  void numAudioInputChanged(ConnectableNode *, int /*newNumInput*/) override;
  void numAudioOutputChanged(ConnectableNode *, int /*newNumOutput*/) override;


  void handleCommandMessage(int id) override;
  enum {
    updatePresetCBID,
    repaintId,
    audioInputChangedId,
    audioOutputChangedId
  }DrawingCommand;

  JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(ConnectableNodeHeaderUI)




};




#endif  // CONNECTABLENODEHEADERUI_H_INCLUDED
