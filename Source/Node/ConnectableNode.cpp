/*
 ==============================================================================

 Copyright © Organic Orchestra, 2017

 This file is part of LGML. LGML is a software to manipulate sound in real-time

 This program is free software; you can redistribute it and/or modify
 it under the terms of the GNU General Public License as published by
 the Free Software Foundation (version 3 of the License).

 This program is distributed in the hope that it will be useful,
 but WITHOUT ANY WARRANTY; without even the implied warranty of
 MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.

 ==============================================================================
 */

#include "ConnectableNode.h"

#include "../Audio/AudioHelpers.h"
#include "NodeContainer/NodeContainer.h"


bool isEngineLoadingFile();


ConnectableNode::ConnectableNode (const String& name, bool _hasMainAudioControl):
    parentNodeContainer (nullptr),
    hasMainAudioControl (_hasMainAudioControl),
    canBeRemovedByUser (true),
    ParameterContainer (name),
    userCanAccessInputs (true),
    userCanAccessOutputs (true),
    outputVolume (nullptr)
{
    //set Params
    ParameterContainer::nameParam->isEditable = true;
    descriptionParam = addNewParameter<StringParameter> ("Description", "Set a custom description for this node.", "");
    enabledParam = addNewParameter<BoolParameter> ("Enabled", "Enable processing of the node.\nDisable will bypass the node and pass the audio flux to the output", true);



    descriptionParam->isPresettable = false;
    enabledParam->isPresettable = true;


    //Audio
    if (hasMainAudioControl)
    {
        outputVolume = addNewParameter<FloatParameter> ("masterVolume", "master volume for this node", DB0_FOR_01);
        //  bypass = addNewParameter<BoolParameter>("Bypass", "by-pass current node, letting audio pass thru", false);
    }

    /*
      setInputChannelName(0, "Main Left");
      setInputChannelName(1, "Main Right");
      setOutputChannelName(0, "Main Left");
      setOutputChannelName(1, "Main Right");
      */

    
}

ConnectableNode::~ConnectableNode()
{
    if (parentNodeContainer)
        remove();
    ConnectableNode::masterReference.clear();
    parentNodeContainer = nullptr;
}


void ConnectableNode::setParentNodeContainer (NodeContainer* _parentNodeContainer)
{
    jassert(parentNodeContainer==nullptr ||_parentNodeContainer==nullptr );
    parentNodeContainer = _parentNodeContainer;


}

NodeContainer*   const ConnectableNode::getParentNodeContainer() const
{
    return parentNodeContainer;
}




bool ConnectableNode::hasAudioInputs()
{
    //to override
    return false;

}

bool ConnectableNode::hasAudioOutputs()
{
    //to override
    return false;

}





void ConnectableNode::onContainerParameterChanged ( ParameterBase* p)
{

}


void ConnectableNode::remove()
{
    if (parentNodeContainer)
    {
        parentNodeContainer->removeNode (this);
    }
    else
    {
        jassertfalse;
    }

}

void ConnectableNode::clear()
{
      //ControllableContainer::clearContainer(true);
    //to override
}


void ConnectableNode::parameterValueChanged ( ParameterBase* p, ParameterBase::Listener * notifier){ // force calling listener even if notifier is this
    nodeListeners.call (&ConnectableNodeListener::nodeParameterChanged, this, p);
    ParameterContainer::parameterValueChanged(p,notifier);
}


void ConnectableNode::newMessage(const ParameterBase::ParamWithValue &pv){
    nodeListeners.call (&ConnectableNodeListener::nodeParameterChangedAsync, this, pv.parameter);
    ParameterContainer::newMessage(pv);
}


/////////////////////////////  AUDIO




void ConnectableNode::setInputChannelNames (int startChannel, StringArray names)
{
    for (int i = startChannel; i < startChannel + names.size(); i++)
    {
        setInputChannelName (i, names[i]);
    }
}

void ConnectableNode::setOutputChannelNames (int startChannel, StringArray names)
{
    for (int i = startChannel; i < startChannel + names.size(); i++)
    {
        setOutputChannelName (i, names[i]);
    }
}

void ConnectableNode::setInputChannelName (int channelIndex, const String& name)
{
    while (inputChannelNames.size() < (channelIndex + 1))
    {
        inputChannelNames.add ("");
    }

    inputChannelNames.set (channelIndex, name);
}

void ConnectableNode::setOutputChannelName (int channelIndex, const String& name)
{
    while (outputChannelNames.size() < (channelIndex + 1))
    {
        outputChannelNames.add ("");
    }

    outputChannelNames.set (channelIndex, name);
}

String ConnectableNode::getInputChannelName (int channelIndex)
{
    String defaultName = "Input " + String (channelIndex + 1);

    if (channelIndex < 0 || channelIndex >= inputChannelNames.size()) return defaultName;

    String s = inputChannelNames[channelIndex];

    if (s.isNotEmpty()) return s;

    return defaultName;
}

String ConnectableNode::getOutputChannelName (int channelIndex)
{
    String defaultName = "Output " + String (channelIndex + 1);

    if (channelIndex < 0 || channelIndex >= outputChannelNames.size()) return defaultName;

    String s = outputChannelNames[channelIndex];

    if (s.isNotEmpty()) return s;

    return defaultName;
}

