/*
  ==============================================================================

    ConnectableNodeAudioCtlUI.h
    Created: 4 May 2016 5:18:01pm
    Author:  Martin Hermant

  ==============================================================================
*/

#ifndef CONNECTABLENODEAUDIOCTLUI_H_INCLUDED
#define CONNECTABLENODEAUDIOCTLUI_H_INCLUDED



class FloatSliderUI;
class ConnectableNode;
class ConnectableNodeUI;
#include "JuceHeader.h"//keep

class ConnectableNodeAudioCtlUI:public Component{

public:
    ConnectableNodeAudioCtlUI();

    void setNodeAndNodeUI(ConnectableNode * _node, ConnectableNodeUI * _nodeUI);
	virtual void init();

    void resized() override;

    ScopedPointer<FloatSliderUI>  outputVolume;
    //ScopedPointer<BoolToggleUI>  bypassUI;



};


#endif  // CONNECTABLENODEAUDIOCTLUI_H_INCLUDED
