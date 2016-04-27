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
    float step = area.getHeight()/outputBusUIs.size();
        const int pad = 3;
    for(auto & o:outputBusUIs){
        o->setBounds(area.removeFromTop(step).reduced(pad));
    }
}


void AudioMixerNodeUI::controllableContainerAdded(ControllableContainer * c) {
    if(AudioMixerNode::AudioMixerAudioProcessor::OutputBus * o = dynamic_cast<AudioMixerNode::AudioMixerAudioProcessor::OutputBus*>(c)){
        OutputBusUI * oo =new OutputBusUI(o);
        outputBusUIs.add(oo);
        addAndMakeVisible(oo);
        resized();
    }

}
void AudioMixerNodeUI::controllableContainerRemoved(ControllableContainer *cc){
    if(AudioMixerNode::AudioMixerAudioProcessor::OutputBus * o = dynamic_cast<AudioMixerNode::AudioMixerAudioProcessor::OutputBus*>(cc)){
        int idx = 0 ;
        for(auto & oo:outputBusUIs){
            if(oo->outputIdx==o->outputIndex){
                outputBusUIs.remove(idx);
                // outputIdx should be a UID so safely return here
                return;
            }
            idx++;
        }
    }
}


//=============================
// OutputBusUI



void AudioMixerNodeUI::OutputBusUI::setNumInput(int numInput){
    int lastSize = inputVolumes.size();
    if(numInput>lastSize){
        for(int  i = lastSize ; i < numInput;i++ ){
            controllableAdded(owner->volumes[i]);
        }
    }
    else if(numInput<lastSize){
        inputVolumes.removeRange(numInput-1, lastSize-numInput);
    }
    resized();
}



void AudioMixerNodeUI::OutputBusUI::resized() {
    if(inputVolumes.size()==0)return;
    Rectangle<int> area = getLocalBounds();
    float step = area.getWidth()/inputVolumes.size();
    const int pad = 2;
    for(auto & o:inputVolumes){
        o->setBounds(area.removeFromLeft(step).reduced(pad));
    }
}

void AudioMixerNodeUI::OutputBusUI::controllableAdded(Controllable * c) {
    if(FloatParameter * f = dynamic_cast<FloatParameter*>(c)){
        FloatSliderUI *v = new FloatSliderUI(f);
        v->orientation = FloatSliderUI::Direction::VERTICAL;
        inputVolumes.add(v);
        addAndMakeVisible(v);
                resized();
    }
};
void AudioMixerNodeUI::OutputBusUI::controllableRemoved(Controllable * c) {
    if(FloatParameter * f = dynamic_cast<FloatParameter*>(c)){
        inputVolumes.removeLast();
        resized();
    }
};
