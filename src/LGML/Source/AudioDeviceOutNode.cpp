/*
==============================================================================

AudioDeviceOutNode.cpp
Created: 7 Mar 2016 8:04:38pm
Author:  Martin Hermant

==============================================================================
*/

#include "AudioDeviceOutNode.h"
#include "NodeBaseUI.h"
#include "AudioDeviceOutNodeUI.h"

AudioDeviceManager& getAudioDeviceManager();

AudioDeviceOutNode::AudioDeviceOutNode() :
	NodeBase("AudioDeviceOut",NodeType::AudioDeviceOutType, false),
	AudioGraphIOProcessor(AudioProcessorGraph::AudioGraphIOProcessor::IODeviceType::audioOutputNode)
{
	canHavePresets = false;
	
	addNodeBaseListener(this);

	NodeBase::busArrangement.outputBuses.clear();

	getAudioDeviceManager().addChangeListener(this);
	
	for (int i = 0; i < NodeBase::getTotalNumInputChannels(); i++) addMute();

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
	if (!enabledParam->boolValue()) return;

	int numChannels = buffer.getNumChannels();
	int numSamples = buffer.getNumSamples();
	for (int i = 0; i < numChannels; i++)
	{
		float gain = outMutes[i]->boolValue() ? 0.f : 1.f;
		buffer.applyGain(i, 0, numSamples, gain);
	}

	AudioProcessorGraph::AudioGraphIOProcessor::processBlock(buffer, midiMessages);
}


void AudioDeviceOutNode::addMute()
{
	BoolParameter * p = addBoolParameter(String(outMutes.size() + 1), "Mute if disabled", false);
	p->setCustomShortName(String("mute") + String(outMutes.size() + 1));
	outMutes.add(p);
}

void AudioDeviceOutNode::removeMute()
{
	BoolParameter * b = outMutes[outMutes.size() - 1];
	removeControllable(b);
}

ConnectableNodeUI * AudioDeviceOutNode::createUI() {
	NodeBaseUI * ui = new NodeBaseUI(this, new AudioDeviceOutNodeContentUI());
	return ui;

}

void AudioDeviceOutNode::audioInputAdded(NodeBase *, int)
{
	DBG("Output added in Node");
	addMute();
}

void AudioDeviceOutNode::audioInputRemoved(NodeBase *, int)
{
	removeMute();
}