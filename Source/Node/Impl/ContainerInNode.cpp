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
    canHavePresets = false;
    userCanAccessInputs = false;

    numChannels = addNewParameter<IntParameter> ("Num Audio Inputs", "Number of input channels for this container", 2, 0, 100);
    numInputData = addNewParameter<IntParameter> ("Num Data Inputs", "Number of data inputs for this container", 0, 0, 100);

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
    NodeBase::setParentNodeContainer (nc);
    setPreferedNumAudioInput (0);
    setPreferedNumAudioOutput (nc->getTotalNumInputChannels());
}

//DATA

void ContainerInNode::processInputDataChanged (Data* d)
{
    Data* od = getOutputDataByName (d->name);
    od->updateFromSourceData (d);
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

void ContainerInNode::onContainerParameterChanged (Parameter* p)
{
    NodeBase::onContainerParameterChanged (p);

    if (p == numChannels)
    {
        setNumChannels (p->intValue());
    }
    else if (p == numInputData)
    {

        while (inputDatas.size() && (p->intValue() < getTotalNumInputData()))
        {
            //      bool res  =
            removeInputData (inputDatas[inputDatas.size() - 1]->name);
            //      jassert(res);
        }

        while ( p->intValue() > getTotalNumInputData())
        {
            addInputData ("Input Data " + String (inputDatas.size()), DataType::Number);
        }

    }

}
