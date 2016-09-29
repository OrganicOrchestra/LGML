/*
 ==============================================================================

 AudioMixerNodeUI.cpp
 Created: 24 Apr 2016 8:37:34pm
 Author:  Martin Hermant

 ==============================================================================
 */

#include "AudioMixerNodeUI.h"
#include "ConnectableNodeUI.h"

void AudioMixerNodeUI::resized() {
    if(outputBusUIs.size()==0)return;
    Rectangle<int> area = getLocalBounds();
    if(mixerNode->oneToOne->boolValue()){
        int diagoNum = jmin(mixerNode->numberOfInput->intValue(),mixerNode->numberOfOutput->intValue());
        float step = (float)(area.getWidth()/ diagoNum);
        const int pad = 3;
        for(int i = 0 ; i < outputBusUIs.size() ; i ++){
            OutputBusUI * o = outputBusUIs.getUnchecked(i);
            if(i<diagoNum){
                o->setOneVisible(i);
                o->setVisible(true);
                o->setBounds(area.removeFromLeft((int)step).reduced(pad));
            }
            else{
                o->setVisible(false);
            }
        }
    }
    else{

        float step = area.getHeight()/ (float)outputBusUIs.size();
        const int pad = 3;
        for(auto & o:outputBusUIs){
            o->setVisible(true);
            o->setBounds(area.removeFromTop((int)step).reduced(pad));
            o->setAllVisible();
        }
    }
}

AudioMixerNodeUI::~AudioMixerNodeUI() {
    mixerNode->removeConnectableNodeListener(this);
    mixerNode->oneToOne->removeParameterListener(this);
}

void AudioMixerNodeUI::init() {
    mixerNode = (AudioMixerNode*)node.get();
    numAudioOutputChanged(mixerNode, mixerNode->numberOfOutput->intValue());
    numAudioInputChanged(mixerNode, mixerNode->numberOfInput->intValue());

    mixerNode->addConnectableNodeListener(this);
    mixerNode->oneToOne->addParameterListener(this);
    nodeUI->setSize(250, 150);
}

void AudioMixerNodeUI::numAudioInputChanged(ConnectableNode *, int numInput){
    for(auto & b:outputBusUIs){
        b->setNumInput(numInput);
    }
    resized();
};
void AudioMixerNodeUI::numAudioOutputChanged(ConnectableNode *, int newNum){
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
void AudioMixerNodeUI::parameterValueChanged(Parameter * p) {
    if(p==mixerNode->oneToOne)resized();
};





//==============================================================================
// OutputBusUI


void AudioMixerNodeUI::OutputBusUI::setOneVisible(int num){
    visibleChanels.clear();
    visibleChanels.setBit(num);
    updateVisibleChannels();
}

void AudioMixerNodeUI::OutputBusUI::setAllVisible(){
    visibleChanels.setRange(0, inputVolumes.size(), true);
    updateVisibleChannels();
}

int AudioMixerNodeUI::OutputBusUI::getNumOfVisibleChannels(){
    return visibleChanels.countNumberOfSetBits();
}
void AudioMixerNodeUI::OutputBusUI::updateVisibleChannels(){
    for(int i = 0 ; i < inputVolumes.size() ; i++){
        inputVolumes.getUnchecked(i)->setVisible(visibleChanels[i]);
    }
    resized();

}

void AudioMixerNodeUI::OutputBusUI::setNumInput(int numInput){
    int lastSize = inputVolumes.size();
    if(numInput>lastSize){
        for(int  i = lastSize ; i < numInput;i++ ){
            FloatSliderUI *v = new FloatSliderUI(owner->volumes[i]);
            v->orientation = FloatSliderUI::Direction::VERTICAL;
            inputVolumes.add(v);
            addAndMakeVisible(v);
            visibleChanels.setBit(i);

        }
        updateVisibleChannels();
    }
    else if(numInput<lastSize){
        inputVolumes.removeLast(lastSize-numInput,true);
    }
    resized();
}



void AudioMixerNodeUI::OutputBusUI::resized() {
    if(inputVolumes.size()==0)return;
    Rectangle<int> area = getLocalBounds();
    int numToBeDisplayed = getNumOfVisibleChannels();
    float step = area.getWidth()*1.0f/numToBeDisplayed;
    const int pad = 2;
    int idx = 0;
    for(auto & o:inputVolumes){
        if(visibleChanels[idx])
            o->setBounds(area.removeFromLeft((int)step).reduced(pad));
        idx++;
    }
}
