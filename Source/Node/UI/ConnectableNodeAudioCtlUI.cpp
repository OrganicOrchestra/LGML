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


#include "ConnectableNodeAudioCtlUI.h"


#include "../../Controllable/Parameter/UI/BoolToggleUI.h"
#include "../../Controllable/Parameter/UI/FloatSliderUI.h"
#include "../ConnectableNode.h"
#include "ConnectableNodeUI.h"

ConnectableNodeAudioCtlUI::ConnectableNodeAudioCtlUI():
	outputVolume(nullptr)
	//,bypassUI(nullptr)
{


}
void ConnectableNodeAudioCtlUI::resized() {
    Rectangle<int > outCtl = getLocalBounds();
    //if(bypassUI)bypassUI->setBounds(outCtl.removeFromTop(outCtl.getWidth()));
    if(outputVolume)outputVolume->setBounds(outCtl);
}



void ConnectableNodeAudioCtlUI::setNodeAndNodeUI(ConnectableNode * node, ConnectableNodeUI *){
    jassert(node->hasAudioOutputs());
    outputVolume = new FloatSliderUI(node->outputVolume);
    outputVolume->orientation = FloatSliderUI::Direction::VERTICAL;
    addAndMakeVisible(outputVolume);
    //bypassUI = node->bypass->createToggle();
    //addAndMakeVisible(bypassUI);

	init(); //for child override
}

void ConnectableNodeAudioCtlUI::init()
{
	//to be override by child classes
}
