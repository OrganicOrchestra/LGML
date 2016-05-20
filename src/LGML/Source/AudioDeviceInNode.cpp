/*
==============================================================================

AudioDeviceInNode.cpp
Created: 7 Mar 2016 8:03:48pm
Author:  Martin Hermant

==============================================================================
*/

#include "AudioDeviceInNode.h"
#include "NodeBaseUI.h"

AudioDeviceManager& getAudioDeviceManager();

AudioDeviceInNode::AudioDeviceInNode() :
	NodeBase("AudioDeviceIn",NodeType::AudioDeviceInType),
	AudioGraphIOProcessor(AudioProcessorGraph::AudioGraphIOProcessor::IODeviceType::audioInputNode)
{
	canHavePresets = false;
	//hasMainAudioControl = false;

    NodeBase::busArrangement.inputBuses.clear();
	getAudioDeviceManager().addChangeListener(this);
	this->updateIO();
}

AudioDeviceInNode::~AudioDeviceInNode() {
	getAudioDeviceManager().removeChangeListener(this);
}

void AudioDeviceInNode::processBlockInternal(AudioBuffer<float>& buffer, MidiBuffer & midiMessages) {
	AudioProcessorGraph::AudioGraphIOProcessor::processBlock(buffer, midiMessages);
}

void AudioDeviceInNode::changeListenerCallback(ChangeBroadcaster*) {
	updateIO();
}


void AudioDeviceInNode::updateIO() {
	AudioIODevice * ad = getAudioDeviceManager().getCurrentAudioDevice();
	if (ad == nullptr) return;
	setPreferedNumAudioOutput(ad->getActiveInputChannels().countNumberOfSetBits());
}


ConnectableNodeUI * AudioDeviceInNode::createUI() {
	NodeBaseUI * ui = new NodeBaseUI(this);
	return ui;

}