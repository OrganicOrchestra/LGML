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



#include "AudioDeviceOutNode.h"


#include "../../Audio/AudioHelpers.h"
#include "../Manager/NodeManager.h"

REGISTER_NODE_TYPE (AudioDeviceOutNode)
AudioDeviceManager& getAudioDeviceManager();

AudioDeviceOutNode::AudioDeviceOutNode (StringRef name) :
    NodeBase (name, false),
    AudioGraphIOProcessor (AudioProcessorGraph::AudioGraphIOProcessor::IODeviceType::audioOutputNode),
    globalFader(1000,1000,false,1)
{
    //CanHavePresets = false;



    {
        MessageManagerLock ml;
        getAudioDeviceManager().addChangeListener (this);
    }
    AudioIODevice* ad = getAudioDeviceManager().getCurrentAudioDevice();

    desiredNumAudioOutput = addNewParameter<IntParameter> ("numAudioOutput", "desired numAudioOutputs (independent of audio settings)",
                                                           ad ? jmax(2,ad->getActiveInputChannels().countNumberOfSetBits()) : 2, 2, 32);
    lastNumberOfOutputs = 0;

    setPreferedNumAudioInput (desiredNumAudioOutput->intValue());
    setPreferedNumAudioOutput (0);
    globalFader.startFadeIn();
}

void AudioDeviceOutNode::setParentNodeContainer (NodeContainer* parent)
{
    NodeBase::setParentNodeContainer (parent);
    if(parent != NodeManager::getInstance()){
        LOGE(juce::translate("avoid creating AudioDeviceIn/Out in container, unstable behaviour"));
        jassertfalse;
    }

    AudioGraphIOProcessor::setRateAndBufferSizeDetails (NodeBase::getSampleRate(), NodeBase::getBlockSize());
    updateVolMutes();
}

AudioDeviceOutNode::~AudioDeviceOutNode()
{
    MessageManagerLock ml;
    getAudioDeviceManager().removeChangeListener (this);
}

void AudioDeviceOutNode::changeListenerCallback (ChangeBroadcaster*)
{
    updateVolMutes();
};

void AudioDeviceOutNode::onContainerParameterChanged ( ParameterBase* p)
{

    if (p == desiredNumAudioOutput)
    {
        setPreferedNumAudioInput (desiredNumAudioOutput->intValue());
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

void AudioDeviceOutNode::numChannelsChanged (bool isInput)
{
    NodeBase::numChannelsChanged (isInput);
    updateVolMutes();
}

void AudioDeviceOutNode::updateVolMutes()
{

    while (lastNumberOfOutputs < desiredNumAudioOutput->intValue())
    {
        addVolMute();
    }

    while (lastNumberOfOutputs > desiredNumAudioOutput->intValue())
    {
        removeVolMute();
    }


}

void AudioDeviceOutNode::processBlockBypassed(AudioBuffer<float>& buffer, MidiBuffer& ){
    buffer.clear();
}
void AudioDeviceOutNode::processBlockInternal (AudioBuffer<float>& buffer, MidiBuffer& midiMessages)
{
    if (!enabledParam->boolValue()) {
        buffer.clear();
        AudioProcessorGraph::AudioGraphIOProcessor::processBlock (buffer, midiMessages);
        return;
    }

    int numChannels = jmin (NodeBase::getTotalNumInputChannels(), AudioProcessorGraph::AudioGraphIOProcessor::getTotalNumInputChannels());
    int numSamples = buffer.getNumSamples();
    globalFader.incrementFade(numSamples);
    const double globalFaderValue = globalFader.getCurrentFade();
    for (int i = 0; i < numChannels; i++)
    {
        float gain = i < outMutes.size() ? (outMutes[i]->boolValue() ? 0.f : logVolumes[i]*globalFaderValue) : 0.0f;
        buffer.applyGainRamp (i, 0, numSamples, lastVolumes[i], gain);
        lastVolumes.set (i, gain);
    }

    AudioProcessorGraph::AudioGraphIOProcessor::processBlock (buffer, midiMessages);
}


void AudioDeviceOutNode::addVolMute()
{

    BoolParameter* p = addNewParameter<BoolParameter> (String ("mute") + String (outMutes.size() + 1), "Mute if disabled", false);
    p->invertVisuals = true;
    outMutes.add (p);

    FloatParameter* v = addNewParameter<FloatParameter> ("volume" + String (volumes.size()+1), "volume", DB0_FOR_01);
    volumes.add (v);
    lastVolumes.add (0);
    logVolumes.add (float01ToGain (DB0_FOR_01));
    lastNumberOfOutputs++;
}

void AudioDeviceOutNode::removeVolMute()
{
    if (outMutes.size() == 0)return;

    BoolParameter* b = outMutes[outMutes.size() - 1];
    removeControllable (b);
    outMutes.removeAllInstancesOf (b);

    removeControllable (volumes.getLast());
    lastVolumes.removeLast();
    volumes.removeLast();
    logVolumes.removeLast();
    lastNumberOfOutputs--;
}




