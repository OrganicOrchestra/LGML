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

class VuMeter;
class AudioDeviceOutNode;

class AudioDeviceOutNodeContentUI :
	public NodeBaseContentUI,
	public NodeBase::NodeBaseListener,
	public ConnectableNode::ConnectableNodeListener
{
public:
	AudioDeviceOutNodeContentUI();
	virtual ~AudioDeviceOutNodeContentUI();

    OwnedArray<BoolToggleUI> muteToggles;
    OwnedArray<FloatSliderUI> volumes;
    OwnedArray<VuMeter> vuMeters;

	AudioDeviceOutNode * audioOutNode;

	void init() override;

	void resized() override;

	void updateVuMeters();

	void addVuMeter();
	void removeLastVuMeter();

	virtual void nodeParameterChanged(ConnectableNode*, Parameter *) override;

	virtual void numAudioOutputChanged(NodeBase *, int newNum) override;
	//virtual void numAudioOutputChanged() { DBG("Output changed !"); }

};



#endif  // AUDIODEVICEOUTNODEUI_H_INCLUDED
