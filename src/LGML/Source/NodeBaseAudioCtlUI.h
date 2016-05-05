/*
  ==============================================================================

    NodeBaseAudioCtlUI.h
    Created: 4 May 2016 5:18:01pm
    Author:  Martin Hermant

  ==============================================================================
*/

#ifndef NODEBASEAUDIOCTLUI_H_INCLUDED
#define NODEBASEAUDIOCTLUI_H_INCLUDED

#include "JuceHeader.h"


class FloatSliderUI;
class BoolToggleUI;
class NodeBase;
class NodeBaseUI;


class NodeBaseAudioCtlUI:public Component{

public:
    NodeBaseAudioCtlUI();

    void setNodeAndNodeUI(NodeBase * _node, NodeBaseUI * _nodeUI);
	virtual void init();

    void resized() override;

    ScopedPointer<FloatSliderUI>  outputVolume;
    ScopedPointer<BoolToggleUI>  bypassUI;



};


#endif  // NODEBASEAUDIOCTLUI_H_INCLUDED
