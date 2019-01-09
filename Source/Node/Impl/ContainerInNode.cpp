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


#include "ContainerInNode.h"
#include "../NodeContainer/NodeContainer.h"


REGISTER_NODE_TYPE (ContainerInNode)


ContainerInNode::ContainerInNode (StringRef name) :
    NodeBase (name, false),
    AudioGraphIOProcessor (AudioProcessorGraph::AudioGraphIOProcessor::IODeviceType::audioInputNode)
{
    canBeRemovedByUser = false;
    _canHavePresets = false;
    userCanAccessInputs = false;

    numChannels = addNewParameter<IntParameter> ("Num Audio Inputs", "Number of input channels for this container", 2, 0, 100);
    setNumChannels(numChannels->intValue());
    
}

ContainerInNode::~ContainerInNode()
{

}

void ContainerInNode::processBlockInternal (AudioBuffer<float>& buffer, MidiBuffer& midiMessages)
{


    AudioProcessorGraph::AudioGraphIOProcessor::processBlock (buffer, midiMessages);

    // graphs can be fed with bigger amount of channel (if numoutputChannel>numInputChannel)
    // we need to clear them
    for (int i = NodeBase::getTotalNumOutputChannels(); i < buffer.getNumChannels() ; i++)
    {
        buffer.clear (i, 0, buffer.getNumSamples());
    }

};

void ContainerInNode::setParentNodeContainer (NodeContainer* nc)
{
    if(numChannels->intValue()!=AudioGraphIOProcessor::getTotalNumOutputChannels()){
        setNumChannels(numChannels->intValue());
    }
    NodeBase::setParentNodeContainer (nc);
    setPreferedNumAudioInput (0);
//    setPreferedNumAudioOutput (nc->getTotalNumInputChannels());
    if (parentNodeContainer) {
        parentNodeContainer->setPreferedNumAudioInput (NodeBase::getTotalNumOutputChannels());
    }

}


void ContainerInNode::setNumChannels (int num)
{
    setPreferedNumAudioOutput (num);
    AudioGraphIOProcessor::setPlayConfigDetails (0, num, NodeBase::getSampleRate(), NodeBase::getBlockSize());
    jassert (NodeBase::getTotalNumOutputChannels() == AudioGraphIOProcessor::getTotalNumOutputChannels());
    if (parentNodeContainer) {
        parentNodeContainer->setPreferedNumAudioInput (NodeBase::getTotalNumOutputChannels());
    }
}

void ContainerInNode::onContainerParameterChanged ( ParameterBase* p)
{
    NodeBase::onContainerParameterChanged (p);

    if (p == numChannels)
    {
        setNumChannels (p->intValue());
    }


}
