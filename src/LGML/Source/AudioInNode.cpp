/*
  ==============================================================================

    AudioInNode.cpp
    Created: 7 Mar 2016 8:03:48pm
    Author:  Martin Hermant

  ==============================================================================
*/

#include "AudioInNode.h"
#include "NodeBaseUI.h"


AudioInNode::AudioInNode(uint32 nodeId) :
	NodeBase(nodeId, "AudioInNode"),
	AudioGraphIOProcessor(AudioProcessorGraph::AudioGraphIOProcessor::IODeviceType::audioInputNode)
{
	NodeBase::busArrangement.inputBuses.clear();
	getAudioDeviceManager().addChangeListener(this);
	this->updateIO();
}

AudioInNode::~AudioInNode() {
	getAudioDeviceManager().removeChangeListener(this);
}

void AudioInNode::processBlockInternal(AudioBuffer<float>& buffer, MidiBuffer & midiMessages) {
	AudioProcessorGraph::AudioGraphIOProcessor::processBlock(buffer, midiMessages);

}

void AudioInNode::changeListenerCallback (ChangeBroadcaster*) {
    updateIO();
}


void AudioInNode::updateIO(){
    AudioIODevice * ad  =getAudioDeviceManager().getCurrentAudioDevice();
	if (ad == nullptr) return;
    setPreferedNumAudioOutput(ad->getActiveInputChannels().countNumberOfSetBits());
}


NodeBaseUI * AudioInNode::createUI() {
	NodeBaseUI * ui = new NodeBaseUI(this);
	return ui;

}