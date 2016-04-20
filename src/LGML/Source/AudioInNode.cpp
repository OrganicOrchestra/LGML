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


void AudioInNode::changeListenerCallback (ChangeBroadcaster* source) {
    setIOFromAudioDevice();
    nodeManager->updateAudioGraph();
}
void AudioInNode::setIOFromAudioDevice(){
    AudioIODevice * ad  =getAudioDeviceManager().getCurrentAudioDevice();
    audioProcessor->setPlayConfigDetails(0,
                                         ad->getActiveInputChannels().countNumberOfSetBits(),
                                         ad->getCurrentSampleRate(),
                                         ad->getDefaultBufferSize());

};