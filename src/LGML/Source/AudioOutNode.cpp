/*
  ==============================================================================

    AudioOutNode.cpp
    Created: 7 Mar 2016 8:04:38pm
    Author:  Martin Hermant

  ==============================================================================
*/

#include "AudioOutNode.h"
#include "NodeBaseUI.h"

NodeBaseUI * AudioOutNode::createUI() {
    NodeBaseUI * ui = new NodeBaseUI(this);
    return ui;

}


void AudioOutNode::AudioOutProcessor::changeListenerCallback (ChangeBroadcaster*) {
    updateIO();
};

void AudioOutNode::AudioOutProcessor::updateIO(){
    AudioIODevice * ad  =getAudioDeviceManager().getCurrentAudioDevice();
    setPreferedNumAudioInput(ad->getActiveOutputChannels().countNumberOfSetBits());
}

