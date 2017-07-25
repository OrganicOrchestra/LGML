/*
 ==============================================================================

 AudioDeviceOutNodeUI.h
 Created: 25 May 2016 9:31:21am
 Author:  bkupe

 ==============================================================================
 */

#ifndef AUDIODEVICEOUTNODEUI_H_INCLUDED
#define AUDIODEVICEOUTNODEUI_H_INCLUDED



#include "NodeBaseContentUI.h"
#include "NodeBase.h"
#include "ParameterUI.h"
class FloatSliderUI;
class VuMeter;
class AudioDeviceOutNode;

class AudioDeviceOutNodeContentUI :
public NodeBaseContentUI,
public ConnectableNode::ConnectableNodeListener,
public ChangeListener
{
public:
  AudioDeviceOutNodeContentUI();
  virtual ~AudioDeviceOutNodeContentUI();

  OwnedArray<ParameterUI> muteToggles;
  OwnedArray<FloatSliderUI> volumes;
  OwnedArray<VuMeter> vuMeters;

  AudioDeviceOutNode * audioOutNode;

  void init() override;

  void resized() override;

  void updateVuMeters();

  void addVuMeter();
  void removeLastVuMeter();

  virtual void nodeParameterChanged(ConnectableNode*, Parameter *) override;

  virtual void numAudioOutputChanged(ConnectableNode *, int newNum) override;
  virtual void numAudioInputChanged(ConnectableNode *, int newNum) override;

  void changeListenerCallback (ChangeBroadcaster* source)override;
  //virtual void numAudioOutputChanged() { DBG("Output changed !"); }

};



#endif  // AUDIODEVICEOUTNODEUI_H_INCLUDED
