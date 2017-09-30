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



#include "AudioDeviceInNode.h"

#include "../../Audio/AudioHelpers.h"
#include "../Manager/NodeManager.h"

REGISTER_NODE_TYPE (AudioDeviceInNode)

AudioDeviceManager& getAudioDeviceManager();

AudioDeviceInNode::AudioDeviceInNode (StringRef name) :
    NodeBase (name),
    AudioGraphIOProcessor (AudioProcessorGraph::AudioGraphIOProcessor::IODeviceType::audioInputNode)
{

    //canHavePresets = false;
    hasMainAudioControl = false;

    {
        MessageManagerLock ml;
        getAudioDeviceManager().addChangeListener (this);
    }
    AudioIODevice* ad = getAudioDeviceManager().getCurrentAudioDevice();
    desiredNumAudioInput = addNewParameter<IntParameter> ("numAudioInput", "desired numAudioInputs (independent of audio settings)",
                                                          ad ? jmax(2,ad->getActiveInputChannels().countNumberOfSetBits()) : 2, 2, 32);


    lastNumberOfInputs = 0;
    setPreferedNumAudioOutput (desiredNumAudioInput->intValue());
    setPreferedNumAudioInput (0);



}

AudioDeviceInNode::~AudioDeviceInNode()
{
    MessageManagerLock ml;
    getAudioDeviceManager().removeChangeListener (this);
}

void AudioDeviceInNode::processBlockInternal (AudioBuffer<float>& buffer, MidiBuffer& midiMessages)
{

    int channelsAvailable = AudioProcessorGraph::AudioGraphIOProcessor::getTotalNumOutputChannels();
    int numSamples = buffer.getNumSamples();



    AudioProcessorGraph::AudioGraphIOProcessor::processBlock (buffer, midiMessages);


    //  totalNumOutputChannels = NodeBase::tot();


    float enabledFactor = enabledParam->boolValue() ? 1.f : 0.f;

    int numChannelsToProcess = jmin (NodeBase::getTotalNumOutputChannels(), channelsAvailable);

    for (int i = 0; i < numChannelsToProcess; i++)
    {
        float newVolume = inMutes[i]->boolValue() ? 0.f : logVolumes[i] * enabledFactor;
        buffer.applyGainRamp (i, 0, numSamples, lastVolumes[i], newVolume);
        lastVolumes.set (i, newVolume);

    }

    for (int i = numChannelsToProcess; i < buffer.getNumChannels() ; i++)
    {
        buffer.clear (i, 0, numSamples);
    }


}



void AudioDeviceInNode::setParentNodeContainer (NodeContainer* parent)
{
    NodeBase::setParentNodeContainer (parent);
    if(parent != NodeManager::getInstance()->mainContainer){
        LOG("!!! avoid creating AudioDeviceIn/Out in container, unstable behaviour");
        jassertfalse;
    }
    AudioGraphIOProcessor::setRateAndBufferSizeDetails (NodeBase::getSampleRate(), NodeBase::getBlockSize());
    updateVolMutes();
}


void AudioDeviceInNode::changeListenerCallback (ChangeBroadcaster*)
{
    NodeBase::setRateAndBufferSizeDetails (AudioGraphIOProcessor::getSampleRate(), AudioGraphIOProcessor::getBlockSize());
    updateVolMutes();
}
void AudioDeviceInNode::onContainerParameterChanged (Parameter* p)
{

    if (p == desiredNumAudioInput)
    {
        setPreferedNumAudioOutput (desiredNumAudioInput->intValue());
    }
    else
    {
        int foundIdx = volumes.indexOf ((FloatParameter*)p);

        if (foundIdx >= 0)
        {
            logVolumes.set (foundIdx, float01ToGain (volumes[foundIdx]->floatValue()));
        }
    }

    NodeBase::onContainerParameterChanged (p);

};


void AudioDeviceInNode::updateVolMutes()
{

    while (lastNumberOfInputs < desiredNumAudioInput->intValue())
    {
        addVolMute();
    }

    while (lastNumberOfInputs > desiredNumAudioInput->intValue())
    {
        removeVolMute();
    }


}

void AudioDeviceInNode::numChannelsChanged (bool isInput)
{
    NodeBase::numChannelsChanged (isInput);
    updateVolMutes();
}

void AudioDeviceInNode::addVolMute()
{
    //  const ScopedLock lk (NodeBase::getCallbackLock());
    BoolParameter* p = addNewParameter<BoolParameter> (String (inMutes.size() + 1), "Mute if disabled", false);
    p->setCustomShortName (String ("mute") + String (inMutes.size() + 1));
    p->invertVisuals = true;
    inMutes.add (p);

    FloatParameter* v = addNewParameter<FloatParameter> ("volume" + String (volumes.size()), "volume", DB0_FOR_01);
    volumes.add (v);
    lastVolumes.add (0);
    logVolumes.add (float01ToGain (DB0_FOR_01));
    lastNumberOfInputs++;

}

void AudioDeviceInNode::removeVolMute()
{

    if (inMutes.size() == 0)return;

    //  const ScopedLock lk (parentNodeContainer->getCallbackLock());
    BoolParameter* b = inMutes[inMutes.size() - 1];
    removeControllable (b);
    inMutes.removeAllInstancesOf (b);

    removeControllable (volumes.getLast());
    lastVolumes.removeLast();
    volumes.removeLast();
    logVolumes.removeLast();
    lastNumberOfInputs--;
}

