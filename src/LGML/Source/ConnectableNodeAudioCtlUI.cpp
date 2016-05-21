/*
 ==============================================================================

 ConnectableNodeAudioCtlUI.cpp
 Created: 4 May 2016 5:18:01pm
 Author:  Martin Hermant

 ==============================================================================
 */

#include "ConnectableNodeAudioCtlUI.h"


#include "BoolToggleUI.h"
#include "FloatSliderUI.h"
#include "ConnectableNode.h"
#include "ConnectableNodeUI.h"

ConnectableNodeAudioCtlUI::ConnectableNodeAudioCtlUI():outputVolume(nullptr),bypassUI(nullptr){


}
void ConnectableNodeAudioCtlUI::resized() {
    Rectangle<int > outCtl = getLocalBounds();
    if(bypassUI)bypassUI->setBounds(outCtl.removeFromTop(outCtl.getWidth()));
    if(outputVolume)outputVolume->setBounds(outCtl);
}



void ConnectableNodeAudioCtlUI::setNodeAndNodeUI(ConnectableNode * node, ConnectableNodeUI *){
    jassert(node->hasAudioOutputs());
    outputVolume = node->outputVolume->createSlider();
    outputVolume->orientation = FloatSliderUI::Direction::VERTICAL;
    addAndMakeVisible(outputVolume);
    bypassUI = node->bypass->createToggle();
    addAndMakeVisible(bypassUI);

	init(); //for child override
}

void ConnectableNodeAudioCtlUI::init()
{
	//to be override by child classes
}
