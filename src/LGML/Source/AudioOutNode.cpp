/*
  ==============================================================================

    AudioOutNode.cpp
    Created: 7 Mar 2016 8:04:38pm
    Author:  Martin Hermant

  ==============================================================================
*/

#include "AudioOutNode.h"
#include "NodeBaseUI.h"


AudioOutNode::AudioOutNode(NodeManager * nodeManager, uint32 nodeId) :
	NodeBase(nodeManager, nodeId, "AudioOutNode"),
	AudioGraphIOProcessor(AudioProcessorGraph::AudioGraphIOProcessor::IODeviceType::audioOutputNode)
{
	NodeBase::busArrangement.outputBuses.clear();

	getAudioDeviceManager().addChangeListener(this);
	updateIO();
}

AudioOutNode::~AudioOutNode() {
	getAudioDeviceManager().removeChangeListener(this);
}

void AudioOutNode::changeListenerCallback (ChangeBroadcaster*) {
    updateIO();
};

void AudioOutNode::updateIO(){

    AudioIODevice * ad  = getAudioDeviceManager().getCurrentAudioDevice();
	if (ad == nullptr) return;
    setPreferedNumAudioInput(ad->getActiveOutputChannels().countNumberOfSetBits());
}

void AudioOutNode::processBlockInternal(AudioBuffer<float>& buffer, MidiBuffer & midiMessages) {
	AudioProcessorGraph::AudioGraphIOProcessor::processBlock(buffer, midiMessages);
}



NodeBaseUI * AudioOutNode::createUI() {
	NodeBaseUI * ui = new NodeBaseUI(this);
	return ui;

}
