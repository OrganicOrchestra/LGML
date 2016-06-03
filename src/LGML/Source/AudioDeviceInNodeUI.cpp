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
#include "FloatSliderUI.h"

AudioDeviceInNodeContentUI::AudioDeviceInNodeContentUI() :
	NodeBaseContentUI()
{
	
}

AudioDeviceInNodeContentUI::~AudioDeviceInNodeContentUI()
{
	audioInNode->removeNodeBaseListener(this);
	audioInNode->removeConnectableNodeListener(this);

	while (vuMeters.size() > 0)
	{
		removeLastVuMeter();
	}
}

void AudioDeviceInNodeContentUI::init()
{
	audioInNode = (AudioDeviceInNode *)node.get();
	audioInNode->addNodeBaseListener(this);
	audioInNode->addConnectableNodeListener(this);
	updateVuMeters();

	setSize(240, 80);
}

void AudioDeviceInNodeContentUI::resized()
{
	if (vuMeters.size() == 0) return;

	Rectangle<int> r = getLocalBounds().reduced(10);

	int gap = 0;
	int vWidth = (r.getWidth() / vuMeters.size()) - gap;
	for (int i = 0; i < vuMeters.size();i++)
	{
		Rectangle<int> vr = r.removeFromLeft(vWidth);
		muteToggles[i]->setBounds(vr.removeFromBottom(20).reduced(2));
		vr.removeFromBottom(2);
		vuMeters[i]->setBounds(vr.removeFromLeft(vr.getWidth()/2).reduced(2));
        volumes[i]->setBounds(vr.reduced(2));
		r.removeFromLeft(gap);
	}
}

void AudioDeviceInNodeContentUI::updateVuMeters()
{
    int actualNumberOfMutes = jmin(audioInNode->desiredNumAudioInput->intValue(),
                                   audioInNode->AudioGraphIOProcessor::getTotalNumOutputChannels());
	while (vuMeters.size() <actualNumberOfMutes)
	{
		addVuMeter();
	}

	while (vuMeters.size() > actualNumberOfMutes)
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

    int curVuMeterNum = muteToggles.size();
	BoolToggleUI * b = audioInNode->inMutes[curVuMeterNum]->createToggle();
	b->invertVisuals = true;
	muteToggles.add(b);
	addAndMakeVisible(b);


    FloatSliderUI * vol  = audioInNode->volumes[curVuMeterNum]->createSlider();
    vol->orientation = FloatSliderUI::Direction::VERTICAL;
    volumes.add(vol);
    addAndMakeVisible(vol);
}

void AudioDeviceInNodeContentUI::removeLastVuMeter()
{
    int curVuMeterNum = vuMeters.size() - 1;
	VuMeter * v = vuMeters[curVuMeterNum];
	audioInNode->removeRMSListener(v);
	removeChildComponent(v);
	vuMeters.removeLast();

	removeChildComponent(muteToggles[curVuMeterNum]);
	muteToggles.removeLast();
    
    removeChildComponent(volumes[curVuMeterNum]);
    volumes.removeLast();
}

void AudioDeviceInNodeContentUI::nodeParameterChanged(ConnectableNode *, Parameter * p)
{
	int index = 0;
	for (auto &m : muteToggles)
	{
		if (p == m->parameter.get())
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