/*
  ==============================================================================

    AudioDeviceInNodeUI.cpp
    Created: 25 May 2016 9:31:12am
    Author:  bkupe

  ==============================================================================
*/

#include "AudioDeviceInNodeUI.h"
#include "AudioDeviceInNode.h"
#include "VuMeter.h"

AudioDeviceInNodeContentUI::AudioDeviceInNodeContentUI() :
	NodeBaseContentUI()
{
}

AudioDeviceInNodeContentUI::~AudioDeviceInNodeContentUI()
{
	audioInNode->removeNodeBaseListener(this);
}

void AudioDeviceInNodeContentUI::init()
{
	DBG("HERE");
	audioInNode = (AudioDeviceInNode *)node;
	audioInNode->addNodeBaseListener(this);
	updateVuMeters();
	
}

void AudioDeviceInNodeContentUI::resized()
{
	if (vuMeters.size() == 0) return;

	Rectangle<int> r = getLocalBounds().reduced(10);

	int gap = 5;
	int vWidth = (r.getWidth() / vuMeters.size()) - gap;
	for (auto &v : vuMeters)
	{
		v->setBounds(r.removeFromLeft(vWidth));
		r.removeFromLeft(gap);
	}
}

void AudioDeviceInNodeContentUI::updateVuMeters()
{
	while (vuMeters.size() < audioInNode->AudioGraphIOProcessor::getTotalNumOutputChannels())
	{
		VuMeter * v = new VuMeter(VuMeter::Type::OUT);
		v->targetChannel = vuMeters.size();
		audioInNode->addRMSListener(v);
		addAndMakeVisible(v);
		vuMeters.add(v);
	}

	while (vuMeters.size() > audioInNode->NodeBase::getTotalNumOutputChannels())
	{
		VuMeter * v = vuMeters[vuMeters.size() - 1];
		audioInNode->removeRMSListener(v);
		removeChildComponent(v);
		vuMeters.removeLast();
	}

	DBG("Num audio input changed AUI < " << vuMeters.size());
	resized();
}

void AudioDeviceInNodeContentUI::numAudioOutputChanged(NodeBase *, int)
{
	
	updateVuMeters();
}