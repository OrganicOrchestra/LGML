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


#include "AudioDeviceInNodeUI.h"
#include "AudioDeviceInNode.h"
#include "VuMeter.h"
#include "BoolToggleUI.h"
#include "FloatSliderUI.h"
#include "Style.h"
#include "ParameterUIFactory.h"

AudioDeviceManager& getAudioDeviceManager();


AudioDeviceInNodeContentUI::AudioDeviceInNodeContentUI() :
NodeBaseContentUI()
{
  getAudioDeviceManager().addChangeListener(this);
}

AudioDeviceInNodeContentUI::~AudioDeviceInNodeContentUI()
{
  getAudioDeviceManager().removeChangeListener(this);

  audioInNode->removeConnectableNodeListener(this);


  while (vuMeters.size() > 0)
  {
    removeLastVuMeter();
  }
}

void AudioDeviceInNodeContentUI::init()
{
  audioInNode = (AudioDeviceInNode *)node.get();
  audioInNode->addConnectableNodeListener(this);

  updateVuMeters();
  setDefaultSize(240, 80);

}

void AudioDeviceInNodeContentUI::resized()
{
  if (vuMeters.size() == 0) return;

  Rectangle<int> r = getLocalBounds().reduced(10);

  int gap = 0;
  int vWidth = jmin<int>((r.getWidth() / vuMeters.size()) - gap, 30);
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

  int desiredNumInputs = audioInNode->desiredNumAudioInput->intValue();
  int actualNumberOfMutes = jmin(desiredNumInputs,
                                 audioInNode->AudioGraphIOProcessor::getTotalNumOutputChannels());


  while (vuMeters.size() < desiredNumInputs)
  {
    addVuMeter();
  }

  while (vuMeters.size() > desiredNumInputs)
  {
    removeLastVuMeter();
  }

  for (int i = 0; i < desiredNumInputs; i++)
  {
    bool isActive = i < actualNumberOfMutes;
    volumes[i]->defaultColor = isActive ? PARAMETER_FRONT_COLOR : Colours::lightgrey;
    vuMeters[i]->isActive = isActive;
    volumes[i]->repaint();
  }
  resized();
}

void AudioDeviceInNodeContentUI::addVuMeter()
{
  VuMeter * v = new VuMeter(VuMeter::Type::OUT);
  v->targetChannel = vuMeters.size();
  audioInNode->addRMSChannelListener(v);
  addAndMakeVisible(v);
  vuMeters.add(v);

  int curVuMeterNum = muteToggles.size();
  jassert(curVuMeterNum<audioInNode->volumes.size() );
  auto b = ParameterUIFactory::createDefaultUI(audioInNode->inMutes[curVuMeterNum]);

  muteToggles.add(b);
  addAndMakeVisible(b);


  FloatSliderUI * vol  = new FloatSliderUI(audioInNode->volumes[curVuMeterNum]);
  vol->orientation = FloatSliderUI::Direction::VERTICAL;
  volumes.add(vol);
  addAndMakeVisible(vol);
}

void AudioDeviceInNodeContentUI::removeLastVuMeter()
{
  int curVuMeterNum = vuMeters.size() - 1;
  VuMeter * v = vuMeters[curVuMeterNum];
  audioInNode->removeRMSChannelListener(v);
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
    if (p && (p == m->parameter.get()))
    {
      if(p->boolValue()) vuMeters[index]->setVoldB(0);
    }
    index++;
  }

}

void AudioDeviceInNodeContentUI::numAudioOutputChanged(ConnectableNode *, int)
{
  updateVuMeters();
}


void AudioDeviceInNodeContentUI::changeListenerCallback(ChangeBroadcaster *)
{
  updateVuMeters();
}
