/* Copyright © Organic Orchestra, 2017
*
* This file is part of LGML.  LGML is a software to manipulate sound in realtime
*
* This program is free software; you can redistribute it and/or modify
* it under the terms of the GNU General Public License as published by
* the Free Software Foundation (version 3 of the License).
*
* This program is distributed in the hope that it will be useful,
* but WITHOUT ANY WARRANTY; without even the implied warranty of
* MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
*
*/
#if !ENGINE_HEADLESS && !NON_INCREMENTAL_COMPILATION

#include "AudioDeviceOutNodeUI.h"

#include "AudioDeviceOutNode.h"
#include "../../UI/VuMeter.h"
#include "../../Controllable/Parameter/UI/BoolToggleUI.h"
#include "../../Controllable/Parameter/UI/SliderUI.h"
#include "../../Controllable/Parameter/UI/ParameterUIFactory.h"

AudioDeviceManager& getAudioDeviceManager();

AudioDeviceOutNodeContentUI::AudioDeviceOutNodeContentUI():audioOutNode(nullptr)
{

}

AudioDeviceOutNodeContentUI::~AudioDeviceOutNodeContentUI()
{
    audioOutNode->removeConnectableNodeListener (this);


    while (vuMeters.size() > 0)
    {
        removeLastVuMeter();
    }

    getAudioDeviceManager().removeChangeListener (this);

}

void AudioDeviceOutNodeContentUI::changeListenerCallback (ChangeBroadcaster*)
{
    updateVuMeters();
}

void AudioDeviceOutNodeContentUI::init()
{
    audioOutNode = (AudioDeviceOutNode*)node.get();
    audioOutNode->addConnectableNodeListener (this);

    getAudioDeviceManager().addChangeListener (this);

    updateVuMeters();
    setDefaultSize (100, 80);
    setDefaultPosition(250,100);






}

void AudioDeviceOutNodeContentUI::resized()
{
    if (vuMeters.size() == 0) return;

    Rectangle<int> r = getLocalBounds().reduced (10);

    int gap = 0;
    int vWidth = jmin<int> ((r.getWidth() / vuMeters.size()) - gap, 30);

    for (int i = 0; i < vuMeters.size(); i++)
    {
        Rectangle<int> vr = r.removeFromLeft (vWidth);
        muteToggles[i]->setBounds (vr.removeFromBottom (20).reduced (2));
        vr.removeFromBottom (2);
        vuMeters[i]->setBounds (vr.removeFromLeft (vr.getWidth() / 2).reduced (2));
        volumes[i]->setBounds (vr.reduced (2));
        r.removeFromLeft (gap);
    }
}

void AudioDeviceOutNodeContentUI::updateVuMeters()
{
    int desiredNumOutputs = audioOutNode->desiredNumAudioOutput->intValue();
    int validNumberOfTracks = jmin (audioOutNode->desiredNumAudioOutput->intValue(),
                                    audioOutNode->AudioGraphIOProcessor::getTotalNumInputChannels());

    while (vuMeters.size() < desiredNumOutputs)
    {
        addVuMeter();
    }

    while (vuMeters.size() > desiredNumOutputs)
    {
        removeLastVuMeter();
    }


    for (int i = 0; i < desiredNumOutputs; i++)
    {
        bool isActive = i < validNumberOfTracks;
        volumes[i]->setColour(Slider::trackColourId,isActive ? findColour (Slider::trackColourId) : Colours::darkgrey);
        vuMeters[i]->isActive = isActive;
        volumes[i]->repaint();
    }

    resized();
}

void AudioDeviceOutNodeContentUI::addVuMeter()
{
    VuMeter* v = new VuMeter (VuMeter::Type::IN);
    v->targetChannel = vuMeters.size();
    audioOutNode->addRMSChannelListener (v);
    addAndMakeVisible (v);
    vuMeters.add (v);

    int curVuMeterNum = muteToggles.size();

    auto b = muteToggles.add (ParameterUIFactory::createDefaultUI (audioOutNode->outMutes[curVuMeterNum]));
    b->setCustomText(String(v->targetChannel+1));
    addAndMakeVisible (b);


    FloatSliderUI* vol  = new FloatSliderUI (audioOutNode->volumes[curVuMeterNum]);
    vol->orientation = FloatSliderUI::Direction::VERTICAL;
    volumes.add (vol);
    addAndMakeVisible (vol);

}

void AudioDeviceOutNodeContentUI::removeLastVuMeter()
{
    int curVuMeterNum = vuMeters.size() - 1;
    VuMeter* v = vuMeters[curVuMeterNum];
    audioOutNode->removeRMSChannelListener (v);
    removeChildComponent (v);
    vuMeters.removeLast();

    removeChildComponent (muteToggles[curVuMeterNum]);
    muteToggles.removeLast();

    removeChildComponent (volumes[curVuMeterNum]);
    volumes.removeLast();
}
void AudioDeviceOutNodeContentUI::nodeParameterChangedAsync (ConnectableNode*, ParameterBase* p)
{

    int index = 0;

    for (auto& m : muteToggles)
    {
        if (p == m->parameter)
        {
            if (p && p->boolValue()) vuMeters[index]->setVoldB (0);
        }

        index++;
    }
}

void AudioDeviceOutNodeContentUI::numAudioOutputChanged (ConnectableNode*, int)
{
    updateVuMeters();
}


void AudioDeviceOutNodeContentUI::numAudioInputChanged (ConnectableNode*, int)
{
    updateVuMeters();
}

#endif
