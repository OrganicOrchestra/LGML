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
#include "BoolToggleUI.h"

AudioDeviceInNodeContentUI::AudioDeviceInNodeContentUI() :
	NodeBaseContentUI()
{
	
}

AudioDeviceInNodeContentUI::~AudioDeviceInNodeContentUI()
{
	audioInNode->removeNodeBaseListener(this);
	audioInNode->removeNodeListener(this);

	while (vuMeters.size() > 0)
	{
		removeLastVuMeter();
	}
}

void AudioDeviceInNodeContentUI::init()
{
	audioInNode = (AudioDeviceInNode *)node;
	audioInNode->addNodeBaseListener(this);
	audioInNode->addNodeListener(this);
	updateVuMeters();

	setSize(240, 80);
}

void AudioDeviceInNodeContentUI::resized()
{
	if (vuMeters.size() == 0) return;

	Rectangle<int> r = getLocalBounds().reduced(10);

	int gap = 5;
	int vWidth = (r.getWidth() / vuMeters.size()) - gap;
	for (int i = 0; i < vuMeters.size();i++)
	{
		Rectangle<int> vr = r.removeFromLeft(vWidth);
		muteToggles[i]->setBounds(vr.removeFromBottom(20));
		vr.removeFromBottom(2);
		vuMeters[i]->setBounds(vr);
		r.removeFromLeft(gap);
	}
}

void AudioDeviceInNodeContentUI::updateVuMeters()
{
	while (vuMeters.size() < audioInNode->AudioGraphIOProcessor::getTotalNumOutputChannels())
	{
		addVuMeter();
	}

	while (vuMeters.size() > audioInNode->NodeBase::getTotalNumOutputChannels())
	{
		removeLastVuMeter();
	}
	resized();
}

void AudioDeviceInNodeContentUI::addVuMeter()
{
	VuMeter * v = new VuMeter(VuMeter::Type::OUT);
	v->targetChannel = vuMeters.size();
	audioInNode->addRMSListener(v);
	addAndMakeVisible(v);
	vuMeters.add(v);

	BoolToggleUI * b = audioInNode->inMutes[muteToggles.size()]->createToggle();
	b->invertVisuals = true;
	muteToggles.add(b);
	addAndMakeVisible(b);
}

void AudioDeviceInNodeContentUI::removeLastVuMeter()
{
	VuMeter * v = vuMeters[vuMeters.size() - 1];
	audioInNode->removeRMSListener(v);
	removeChildComponent(v);
	vuMeters.removeLast();

	removeChildComponent(muteToggles[muteToggles.size() - 1]);
	muteToggles.removeLast();
}

void AudioDeviceInNodeContentUI::nodeParameterChanged(ConnectableNode *, Parameter * p)
{
	int index = 0;
	for (auto &m : muteToggles)
	{
		if (p == m->parameter)
		{
			if(p->boolValue()) vuMeters[index]->setVoldB(0);
		}
		index++;
	}
}

void AudioDeviceInNodeContentUI::numAudioOutputChanged(NodeBase *, int)
{
	DBG("Output changed in NodeUI");
	updateVuMeters();
}