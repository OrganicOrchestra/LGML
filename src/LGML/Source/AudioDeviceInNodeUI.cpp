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
	numAudioInputChanged();
	
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

void AudioDeviceInNodeContentUI::numAudioInputChanged()
{
	
	while (vuMeters.size() < audioInNode->AudioGraphIOProcessor::getTotalNumInputChannels())
	{
		VuMeter * v = new VuMeter(VuMeter::Type::IN);
		v->targetChannel = vuMeters.size();
		audioInNode->addRMSListener(v);
		addAndMakeVisible(v);
		vuMeters.add(v);
	}

	while (vuMeters.size() > audioInNode->NodeBase::getTotalNumInputChannels())
	{
		VuMeter * v = vuMeters[vuMeters.size() - 1];
		audioInNode->removeRMSListener(v);
		removeChildComponent(v);
		vuMeters.removeLast();
	}

	DBG("Num audio input changed AUI < " << vuMeters.size());
	resized();
}