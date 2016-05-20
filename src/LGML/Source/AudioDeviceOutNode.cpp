/*
==============================================================================

AudioDeviceOutNode.cpp
Created: 7 Mar 2016 8:04:38pm
Author:  Martin Hermant

==============================================================================
*/

#include "AudioDeviceOutNode.h"
#include "NodeBaseUI.h"

AudioDeviceManager& getAudioDeviceManager();

AudioDeviceOutNode::AudioDeviceOutNode() :
	NodeBase("AudioDeviceOut",NodeType::AudioDeviceOutType),
	AudioGraphIOProcessor(AudioProcessorGraph::AudioGraphIOProcessor::IODeviceType::audioOutputNode)
{
	canHavePresets = false;
	
	AudioProcessor::busArrangement.outputBuses.clear();

	getAudioDeviceManager().addChangeListener(this);
	updateIO();
}

AudioDeviceOutNode::~AudioDeviceOutNode() {
	getAudioDeviceManager().removeChangeListener(this);
}

void AudioDeviceOutNode::changeListenerCallback(ChangeBroadcaster*) {
	updateIO();
};

void AudioDeviceOutNode::updateIO() {

	AudioIODevice * ad = getAudioDeviceManager().getCurrentAudioDevice();
	if (ad == nullptr) return;
	setPreferedNumAudioInput(ad->getActiveOutputChannels().countNumberOfSetBits());
}

void AudioDeviceOutNode::processBlockInternal(AudioBuffer<float>& buffer, MidiBuffer & midiMessages) {
	AudioProcessorGraph::AudioGraphIOProcessor::processBlock(buffer, midiMessages);
}



ConnectableNodeUI * AudioDeviceOutNode::createUI() {
	NodeBaseUI * ui = new NodeBaseUI(this);
	return ui;

}
