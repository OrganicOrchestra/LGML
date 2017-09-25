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


#ifndef CONNECTABLENODEAUDIOCTLUI_H_INCLUDED
#define CONNECTABLENODEAUDIOCTLUI_H_INCLUDED
#pragma once



class ConnectableNode;
class ConnectableNodeUI;

#include "../../JuceHeaderUI.h"//keep

template<typename T> class SliderUI;
class ConnectableNodeAudioCtlUI:public juce::Component{

public:
    ConnectableNodeAudioCtlUI();

    void setNodeAndNodeUI(ConnectableNode * _node, ConnectableNodeUI * _nodeUI);
	virtual void init();

    void resized() override;

    ScopedPointer<SliderUI<double> >  outputVolume;
    //ScopedPointer<BoolToggleUI>  bypassUI;



};


#endif  // CONNECTABLENODEAUDIOCTLUI_H_INCLUDED
