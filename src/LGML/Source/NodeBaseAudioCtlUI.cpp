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



void NodeBaseAudioCtlUI::setNodeAndNodeUI(NodeBase * node, NodeBaseUI * _nodeUI){
    jassert(node->hasAudioOutputs);
    outputVolume = node->audioProcessor->outputVolume->createSlider();
    outputVolume->orientation = FloatSliderUI::Direction::VERTICAL;
    addAndMakeVisible(outputVolume);
    bypassUI = node->audioProcessor->bypass->createToggle();
    addAndMakeVisible(bypassUI);


}