/*
 ==============================================================================

 AudioMixerNodeUI.cpp
 Created: 24 Apr 2016 8:37:34pm
 Author:  Martin Hermant

 ==============================================================================
 */

#include "AudioMixerNodeUI.h"


void AudioMixerNodeUI::resized() {
    if(outputBusUIs.size()==0)return;
    Rectangle<int> area = getLocalBounds();
    float step = area.getHeight()/ (float)outputBusUIs.size();
    const int pad = 3;
    for(auto & o:outputBusUIs){
        o->setBounds(area.removeFromTop((int)step).reduced(pad));
    }
}

AudioMixerNodeUI::~AudioMixerNodeUI() {
	mixerNode->removeNodeAudioProcessorListener(this);
}

void AudioMixerNodeUI::init() {
	 mixerNode = dynamic_cast<AudioMixerNode*>(node);
	 numAudioOutputChanged(mixerNode, mixerNode->numberOfOutput->intValue());
	 numAudioInputChanged(mixerNode, mixerNode->numberOfInput->intValue());

	 mixerNode->addNodeAudioProcessorListener(this);

	 nodeUI->setSize(250, 150);
 }

 void AudioMixerNodeUI::numAudioInputChanged(NodeBase *, int numInput){
    for(auto & b:outputBusUIs){
        b->setNumInput(numInput);
    }
    resized();
};
void AudioMixerNodeUI::numAudioOutputChanged(NodeBase *, int newNum){
    int lastNum = outputBusUIs.size();
    if(newNum>lastNum){
        for(int i = lastNum ; i < newNum ; i++){
            OutputBusUI * oo =new OutputBusUI(mixerNode->outBuses[i]);
            outputBusUIs.add(oo);
            addAndMakeVisible(oo);
        }
    }
    else if(newNum<lastNum){
        outputBusUIs.removeRange(newNum, lastNum-newNum);
    }
    resized();

};




//==============================================================================
// OutputBusUI



void AudioMixerNodeUI::OutputBusUI::setNumInput(int numInput){
    int lastSize = inputVolumes.size();
    if(numInput>lastSize){
        for(int  i = lastSize ; i < numInput;i++ ){
            FloatSliderUI *v = new FloatSliderUI(owner->volumes[i]);
            v->orientation = FloatSliderUI::Direction::VERTICAL;
            inputVolumes.add(v);
            addAndMakeVisible(v);
            resized();

        }
    }
    else if(numInput<lastSize){
        inputVolumes.removeLast(lastSize-numInput,true);
    }
    resized();
}



void AudioMixerNodeUI::OutputBusUI::resized() {
    if(inputVolumes.size()==0)return;
    Rectangle<int> area = getLocalBounds();
    float step = area.getWidth()/(float)inputVolumes.size();
    const int pad = 2;
    for(auto & o:inputVolumes){
        o->setBounds(area.removeFromLeft((int)step).reduced(pad));
    }
}
