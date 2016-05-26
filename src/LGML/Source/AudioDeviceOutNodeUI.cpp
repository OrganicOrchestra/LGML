/*
  ==============================================================================

    AudioDeviceOutNodeUI.cpp
    Created: 25 May 2016 9:31:21am
    Author:  bkupe

  ==============================================================================
*/

#include "AudioDeviceOutNodeUI.h"



#include "AudioDeviceOutNodeUI.h"
#include "AudioDeviceOutNode.h"
#include "VuMeter.h"
#include "BoolToggleUI.h"

AudioDeviceOutNodeContentUI::AudioDeviceOutNodeContentUI() :
	NodeBaseContentUI()
{

}

AudioDeviceOutNodeContentUI::~AudioDeviceOutNodeContentUI()
{
	audioInNode->removeNodeBaseListener(this);
	audioInNode->removeNodeListener(this);

	while (vuMeters.size() > 0)
	{
		removeLastVuMeter();
	}
}

void AudioDeviceOutNodeContentUI::init()
{
	audioInNode = (AudioDeviceOutNode *)node;
	audioInNode->addNodeBaseListener(this);
	audioInNode->addNodeListener(this);
	updateVuMeters();

	setSize(240, 80);
}

void AudioDeviceOutNodeContentUI::resized()
{
	if (vuMeters.size() == 0) return;

	Rectangle<int> r = getLocalBounds().reduced(10);

	int gap = 5;
	int vWidth = (r.getWidth() / vuMeters.size()) - gap;
	for (int i = 0; i < vuMeters.size(); i++)
	{
		Rectangle<int> vr = r.removeFromLeft(vWidth);
		muteToggles[i]->setBounds(vr.removeFromBottom(20));
		vr.removeFromBottom(2);
		vuMeters[i]->setBounds(vr);
		r.removeFromLeft(gap);
	}
}

void AudioDeviceOutNodeContentUI::updateVuMeters()
{
	while (vuMeters.size() < audioInNode->AudioGraphIOProcessor::getTotalNumInputChannels())
	{
		addVuMeter();
	}

	while (vuMeters.size() > audioInNode->NodeBase::getTotalNumInputChannels())
	{
		removeLastVuMeter();
	}
	resized();
}

void AudioDeviceOutNodeContentUI::addVuMeter()
{
	VuMeter * v = new VuMeter(VuMeter::Type::IN);
	v->targetChannel = vuMeters.size();
	audioInNode->addRMSListener(v);
	addAndMakeVisible(v);
	vuMeters.add(v);

	BoolToggleUI * b = audioInNode->outMutes[muteToggles.size()]->createToggle();
	b->invertVisuals = true;
	muteToggles.add(b);
	addAndMakeVisible(b);
}

void AudioDeviceOutNodeContentUI::removeLastVuMeter()
{
	VuMeter * v = vuMeters[vuMeters.size() - 1];
	audioInNode->removeRMSListener(v);
	removeChildComponent(v);
	vuMeters.removeLast();

	removeChildComponent(muteToggles[muteToggles.size() - 1]);
	muteToggles.removeLast();
}

void AudioDeviceOutNodeContentUI::nodeParameterChanged(ConnectableNode *, Parameter * p)
{
	int index = 0;
	for (auto &m : muteToggles)
	{
		if (p == m->parameter)
		{
			if (p->boolValue()) vuMeters[index]->setVoldB(0);
		}
		index++;
	}
}

void AudioDeviceOutNodeContentUI::numAudioOutputChanged(NodeBase *, int)
{
	updateVuMeters();
}