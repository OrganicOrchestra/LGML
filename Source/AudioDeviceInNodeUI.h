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


#ifndef AUDIODEVICEINNODEUI_H_INCLUDED
#define AUDIODEVICEINNODEUI_H_INCLUDED

#include "NodeBaseContentUI.h"
#include "NodeBase.h"
#include "ParameterUI.h"
class FloatSliderUI;

class VuMeter;
class AudioDeviceInNode;

class AudioDeviceInNodeContentUI :
public NodeBaseContentUI,
public ConnectableNode::ConnectableNodeListener,
public ChangeListener
{
public:
  AudioDeviceInNodeContentUI();
  virtual ~AudioDeviceInNodeContentUI();

  OwnedArray<ParameterUI> muteToggles;
  OwnedArray<FloatSliderUI> volumes;
  OwnedArray<VuMeter> vuMeters;

  AudioDeviceInNode * audioInNode;

  void init() override;

  void resized() override;

  void updateVuMeters();

  void addVuMeter();
  void removeLastVuMeter();

  virtual void nodeParameterChanged(ConnectableNode*, Parameter *) override;

  virtual void numAudioOutputChanged(ConnectableNode *, int newNum) override;


  // Inherited via ChangeListener
  virtual void changeListenerCallback(ChangeBroadcaster * source) override;

};

#endif  // AUDIODEVICEINNODEUI_H_INCLUDED
