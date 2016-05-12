/*
 ==============================================================================

 NodeBaseAudioCtlUI.cpp
 Created: 4 May 2016 5:18:01pm
 Author:  Martin Hermant

 ==============================================================================
 */

#include "NodeBaseAudioCtlUI.h"


#include "BoolToggleUI.h"
#include "FloatSliderUI.h"
#include "NodeBase.h"
#include "NodeBaseUI.h"

NodeBaseAudioCtlUI::NodeBaseAudioCtlUI():outputVolume(nullptr),bypassUI(nullptr){


}
void NodeBaseAudioCtlUI::resized() {
    Rectangle<int > outCtl = getLocalBounds();
    if(bypassUI)bypassUI->setBounds(outCtl.removeFromTop(outCtl.getWidth()));
    if(outputVolume)outputVolume->setBounds(outCtl);
}



void NodeBaseAudioCtlUI::setNodeAndNodeUI(NodeBase * node, NodeBaseUI *){
    jassert(node->hasAudioOutputs());
    outputVolume = node->outputVolume->createSlider();
    outputVolume->orientation = FloatSliderUI::Direction::VERTICAL;
    addAndMakeVisible(outputVolume);
    bypassUI = node->bypass->createToggle();
    addAndMakeVisible(bypassUI);

	init(); //for child override
}

void NodeBaseAudioCtlUI::init()
{
	//to be override by child classes
}
