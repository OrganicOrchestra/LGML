/*
==============================================================================

AudioDeviceInNode.cpp
Created: 7 Mar 2016 8:03:48pm
Author:  Martin Hermant

==============================================================================
*/


#include "AudioDeviceInNode.h"
#include "NodeBaseUI.h"
#include "AudioDeviceInNodeUI.h"

AudioDeviceManager& getAudioDeviceManager();

AudioDeviceInNode::AudioDeviceInNode() :
	NodeBase("AudioDeviceIn",NodeType::AudioDeviceInType),
	AudioGraphIOProcessor(AudioProcessorGraph::AudioGraphIOProcessor::IODeviceType::audioInputNode)
{
	canHavePresets = false;
	hasMainAudioControl = false;

	addNodeBaseListener(this);
    NodeBase::busArrangement.inputBuses.clear();
	getAudioDeviceManager().addChangeListener(this);
	
	for (int i = 0; i < NodeBase::getTotalNumOutputChannels(); i++) addMute();

	this->updateIO();

}

AudioDeviceInNode::~AudioDeviceInNode() {
	removeNodeBaseListener(this);
	getAudioDeviceManager().removeChangeListener(this);
}

void AudioDeviceInNode::processBlockInternal(AudioBuffer<float>& buffer, MidiBuffer & midiMessages) {
	
	
	AudioProcessorGraph::AudioGraphIOProcessor::processBlock(buffer, midiMessages);

	int numChannels = buffer.getNumChannels();
	int numSamples = buffer.getNumSamples();
	float enabledFactor = enabledParam->boolValue()?1.f:0.f;
	for (int i = 0; i < numChannels; i++) 
	{
		float gain = inMutes[i]->boolValue() ? 0.f : 1.f;
		buffer.applyGain(i, 0, numSamples, gain*enabledFactor);
	}
	
	
}

void AudioDeviceInNode::changeListenerCallback(ChangeBroadcaster*) {
	updateIO();
}


void AudioDeviceInNode::updateIO() {
	AudioIODevice * ad = getAudioDeviceManager().getCurrentAudioDevice();
	if (ad == nullptr) return;
	setPreferedNumAudioOutput(ad->getActiveInputChannels().countNumberOfSetBits());
}



void AudioDeviceInNode::addMute()
{
	BoolParameter * p = addBoolParameter(String(inMutes.size() + 1), "Mute if disabled", false);
	p->setCustomShortName(String("mute") + String(inMutes.size() + 1));
	inMutes.add(p);
}

void AudioDeviceInNode::removeMute()
{
	BoolParameter * b = inMutes[inMutes.size() - 1];
	removeControllable(b);
}

ConnectableNodeUI * AudioDeviceInNode::createUI() {
	NodeBaseUI * ui = new NodeBaseUI(this,new AudioDeviceInNodeContentUI());
	return ui;

}

void AudioDeviceInNode::audioOutputAdded(NodeBase *, int)
{
	DBG("Output added in Node");
	addMute();
}

void AudioDeviceInNode::audioOutputRemoved(NodeBase *, int)
{
	removeMute();
}
