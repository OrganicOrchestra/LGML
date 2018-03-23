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


#include "ContainerOutNode.h"

#include "../NodeContainer/NodeContainer.h"


REGISTER_NODE_TYPE (ContainerOutNode)
ContainerOutNode::ContainerOutNode (StringRef name) :
    NodeBase (name, false),
    AudioProcessorGraph::AudioGraphIOProcessor (AudioProcessorGraph::AudioGraphIOProcessor::IODeviceType::audioOutputNode)
{
    canBeRemovedByUser = false;
    canHavePresets = false;
    userCanAccessOutputs = false;

    numChannels = addNewParameter<IntParameter> ("Num Audio Outputs", "Number of output channels for this container", 2, 0, 100);
    
    setNumChannels(numChannels->intValue());
}

ContainerOutNode::~ContainerOutNode()
{
}


void ContainerOutNode::setParentNodeContainer (NodeContainer* nc)
{

    NodeBase::setParentNodeContainer (nc);
    if(numChannels->intValue()!=AudioGraphIOProcessor::getTotalNumInputChannels()){
        setNumChannels(numChannels->intValue());
    }
    setPreferedNumAudioOutput (0);
    setPreferedNumAudioInput (AudioGraphIOProcessor::getTotalNumInputChannels());
    if (parentNodeContainer) {
        parentNodeContainer->setPreferedNumAudioOutput (AudioGraphIOProcessor::getTotalNumInputChannels());
    }

}


void ContainerOutNode::processBlockInternal (AudioBuffer<float>& buffer, MidiBuffer& midiMessages)
{
    AudioProcessorGraph::AudioGraphIOProcessor::processBlock (buffer, midiMessages);
};




void ContainerOutNode::setNumChannels (int num)
{
    setPreferedNumAudioInput (num);
    AudioGraphIOProcessor::setPlayConfigDetails (num, 0, NodeBase::getSampleRate(), NodeBase::getBlockSize());
    jassert(NodeBase::getTotalNumInputChannels()==num);
    jassert (NodeBase::getTotalNumInputChannels() == AudioGraphIOProcessor::getTotalNumInputChannels());

    if (parentNodeContainer) {parentNodeContainer->setPreferedNumAudioOutput (AudioGraphIOProcessor::getTotalNumInputChannels());}
}

void ContainerOutNode::onContainerParameterChanged ( ParameterBase* p)
{
    NodeBase::onContainerParameterChanged (p);

    if (p == numChannels)
    {
        setNumChannels (p->value);
    }



}
