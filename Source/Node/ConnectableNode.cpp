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


#include "ConnectableNode.h"

#include "../Audio/AudioHelpers.h"
#include "NodeContainer/NodeContainer.h"


bool isEngineLoadingFile();


ConnectableNode::ConnectableNode(const String & name, bool _hasMainAudioControl):
parentNodeContainer(nullptr),
hasMainAudioControl(_hasMainAudioControl),
canBeRemovedByUser(true),
ParameterContainer(name),
userCanAccessInputs(true),
userCanAccessOutputs(true),
outputVolume(nullptr)
{
  //set Params
  ParameterContainer::nameParam->isEditable=true;
  descriptionParam = addNewParameter<StringParameter>("Description", "Set a custom description for this node.", "Description");
  enabledParam = addNewParameter<BoolParameter>("Enabled", "Enable processing of the node.\nDisable will bypass the node and pass the audio flux to the output", true);
  
  nodePosition = addNewParameter<Point2DParameter<int>>("nodePosition", "position on canvas",0,0,Array<var>{0,0});

  nodeSize = addNewParameter<Point2DParameter<int>>("nodeSize", "Node Size", 180,100,Array<var>{0,0});

  miniMode = addNewParameter<BoolParameter>("miniMode", "Mini Mode", false);
  
  nodePosition->isControllableExposed = false;
  nodeSize->isControllableExposed = false;

  nodePosition->isPresettable = false;
  nodeSize->isPresettable = false;

  nodePosition->hideInEditor = true;
	 nodeSize->hideInEditor = true;

  descriptionParam->isPresettable = false;
  enabledParam->isPresettable = false;


  //Audio
  if(hasMainAudioControl){
		  outputVolume = addNewParameter<FloatParameter>("masterVolume", "master volume for this node", DB0_FOR_01);
    //  bypass = addNewParameter<BoolParameter>("Bypass", "by-pass current node, letting audio pass thru", false);
  }

  /*
	  setInputChannelName(0, "Main Left");
	  setInputChannelName(1, "Main Right");
	  setOutputChannelName(0, "Main Left");
	  setOutputChannelName(1, "Main Right");
	  */

  //allow for all nested container to have all parameters save in node's preset (except node container, see nodecontainer's constructor)
  presetSavingIsRecursive = true;
}

ConnectableNode::~ConnectableNode()
{
  if(parentNodeContainer)
    remove();


#warning removed explicit behaviour from ben
//  //@Martin :: must do this here (doubling with the one ControllableContainer::clear) to get right preset filter, because getPresetFilter is overriden and when calling getPresetFilter() from ControllableContainer::clear, it doesn't return the overriden method..)
//
//	cleanUpPresets();

  masterReference.clear();
  parentNodeContainer = nullptr;
}


void ConnectableNode::setParentNodeContainer(NodeContainer * _parentNodeContainer) 
{
  parentNodeContainer = _parentNodeContainer;


}

NodeContainer  * const ConnectableNode::getParentNodeContainer() const{
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

bool ConnectableNode::hasDataInputs()
{
  //to override
  return false;
}

bool ConnectableNode::hasDataOutputs()
{
  //to override
  return false;
}




void ConnectableNode::onContainerParameterChanged(Parameter * p)
{
  nodeListeners.call(&ConnectableNodeListener::nodeParameterChanged, this, p);
}


void ConnectableNode::remove()
{
  if(parentNodeContainer){
    parentNodeContainer->removeNode(this);
  }
  else{
    jassertfalse;
  }
  
}

void ConnectableNode::clear()
{
  //  ControllableContainer::clear();
  //to override
}







/////////////////////////////  AUDIO




void ConnectableNode::setInputChannelNames(int startChannel, StringArray names)
{
  for (int i = startChannel; i < startChannel + names.size(); i++)
  {
    setInputChannelName(i, names[i]);
  }
}

void ConnectableNode::setOutputChannelNames(int startChannel, StringArray names)
{
  for (int i = startChannel; i < startChannel + names.size(); i++)
  {
    setOutputChannelName(i, names[i]);
  }
}

void ConnectableNode::setInputChannelName(int channelIndex, const String & name)
{
  while (inputChannelNames.size() < (channelIndex + 1))
  {
    inputChannelNames.add(String::empty);
  }

  inputChannelNames.set(channelIndex, name);
}

void ConnectableNode::setOutputChannelName(int channelIndex, const String & name)
{
  while (outputChannelNames.size() < (channelIndex + 1))
  {
    outputChannelNames.add(String::empty);
  }

  outputChannelNames.set(channelIndex, name);
}

String ConnectableNode::getInputChannelName(int channelIndex)
{
  String defaultName = "Input " + String(channelIndex+1);
  if (channelIndex < 0 || channelIndex >= inputChannelNames.size()) return defaultName;

  String s = inputChannelNames[channelIndex];
  if (s.isNotEmpty()) return s;
  return defaultName;
}

String ConnectableNode::getOutputChannelName(int channelIndex)
{
  String defaultName = "Output " + String(channelIndex+1);
  if (channelIndex < 0 || channelIndex >= outputChannelNames.size()) return defaultName;

  String s = outputChannelNames[channelIndex];
  if (s.isNotEmpty()) return s;
  return defaultName;
}



/////////////////////////////  DATA

Data * ConnectableNode::getInputData(int)
{
  //to override
  return nullptr;
}

Data * ConnectableNode::getOutputData(int)
{
  //to override
  return nullptr;
}

int ConnectableNode::getTotalNumInputData()
{
  //to override
  return 0;
}

int ConnectableNode::getTotalNumOutputData()
{
  //to override
  return 0;
}

StringArray ConnectableNode::getInputDataInfos()
{
  return StringArray();
}

StringArray ConnectableNode::getOutputDataInfos()
{
  return StringArray();
}

Data::DataType ConnectableNode::getInputDataType(const String & , const String & )
{
  //to override
  return Data::DataType::Unknown;
}

Data::DataType ConnectableNode::getOutputDataType(const String & , const String & )
{
  //to override
  return Data::DataType::Unknown;
}

Data * ConnectableNode::getOutputDataByName(const String & )
{
  //to override
  return nullptr;
}

Data * ConnectableNode::getInputDataByName(const String & )
{
  //to override
  return nullptr;
}
