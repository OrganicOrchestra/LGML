/*
  ==============================================================================

    AudioInNode.cpp
    Created: 7 Mar 2016 8:03:48pm
    Author:  Martin Hermant

  ==============================================================================
*/

#include "AudioInNode.h"
#include "NodeBaseUI.h"

NodeBaseUI * AudioInNode::createUI() {
    NodeBaseUI * ui = new NodeBaseUI(this);
    return ui;

}


void AudioInNode::AudioInProcessor::changeListenerCallback (ChangeBroadcaster*) {
    updateIO();
}


void AudioInNode::AudioInProcessor::updateIO(){
    AudioIODevice * ad  =getAudioDeviceManager().getCurrentAudioDevice();
    setPreferedNumAudioOutput(ad->getActiveInputChannels().countNumberOfSetBits());
}