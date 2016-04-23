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


void AudioOutNode::changeListenerCallback (ChangeBroadcaster*) {
    setIOFromAudioDevice();
    nodeManager->updateAudioGraph();
};

void AudioOutNode::setIOFromAudioDevice(){
    AudioIODevice * ad  =getAudioDeviceManager().getCurrentAudioDevice();
    audioProcessor->setPlayConfigDetails(ad->getActiveOutputChannels().countNumberOfSetBits(),
                                         0,
                                         ad->getCurrentSampleRate(),
                                         ad->getDefaultBufferSize());

}
