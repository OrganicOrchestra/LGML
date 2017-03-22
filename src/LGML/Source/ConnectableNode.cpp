/*
 ==============================================================================

 ConnectableNode.cpp
 Created: 18 May 2016 11:33:58pm
 Author:  bkupe

 ==============================================================================
 */

#include "ConnectableNode.h"

#include "ConnectableNodeUI.h"
#include "AudioHelpers.h"
#include "NodeContainer.h"


bool isEngineLoadingFile();


ConnectableNode::ConnectableNode(const String & name, NodeType _type, bool _hasMainAudioControl):
parentNodeContainer(nullptr),
hasMainAudioControl(_hasMainAudioControl),
type(_type),
canBeRemovedByUser(true),
ControllableContainer(name),
userCanAccessInputs(true),
userCanAccessOutputs(true),
audioNode(nullptr),
outputVolume(nullptr)
{
  //set Params
  
  descriptionParam = addStringParameter("Description", "Set a custom description for this node.", "Description");
  enabledParam = addBoolParameter("Enabled", "Enable processing of the node.\nDisable will bypass the node and pass the audio flux to the output", true);
  
  xPosition = addFloatParameter("xPosition", "x position on canvas", 0, 0, 99999);
  yPosition = addFloatParameter("yPosition", "y position on canvas", 0, 0, 99999);
  nodeWidth = addFloatParameter("nodeWidth", "Node Width", 180, 50, 500);
  nodeHeight = addFloatParameter("nodeHeight", "Node Height", 100, 50, 500);
  miniMode = addBoolParameter("miniMode", "Mini Mode", false);
  
  xPosition->isControllableExposed = false;
  yPosition->isControllableExposed = false;
  nodeWidth->isControllableExposed = false;
  nodeHeight->isControllableExposed = false;


  xPosition->isPresettable = false;
  yPosition->isPresettable = false;
  nodeWidth->isPresettable = false;
  nodeHeight->isPresettable = false;

  descriptionParam->isPresettable = false;
  enabledParam->isPresettable = false;


  //Audio
  if(hasMainAudioControl){
		  outputVolume = addFloatParameter("masterVolume", "master volume for this node", DB0_FOR_01);
    //  bypass = addBoolParameter("Bypass", "by-pass current node, letting audio pass thru", false);
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
  //@Martin :: must do this here (doubling with the one ControllableContainer::clear) to get right preset filter, because getPresetFilter is overriden and when calling getPresetFilter() from ControllableContainer::clear, it doesn't return the overriden method..)
	cleanUpPresets();

  masterReference.clear();
  parentNodeContainer = nullptr;
}


void ConnectableNode::setParentNodeContainer(NodeContainer * _parentNodeContainer)
{

  addToAudioGraph(_parentNodeContainer->getAudioGraph());
  parentNodeContainer = _parentNodeContainer;


}

AudioProcessorGraph::Node * ConnectableNode::getAudioNode(bool)
{
  jassert(audioNode->getProcessor() == getAudioProcessor());
  return audioNode;
}




bool ConnectableNode::hasAudioInputs()
{
  //to override
  if (getAudioNode(true) == nullptr) return false;
  return getAudioNode(true)->getProcessor()->getTotalNumInputChannels() > 0;
}

bool ConnectableNode::hasAudioOutputs()
{
  //to override
  if (getAudioNode(false) == nullptr) return false;
  return getAudioNode(false)->getProcessor()->getTotalNumOutputChannels() > 0;
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

String ConnectableNode::getPresetFilter()
{
  return NodeFactory::nodeToString(this) + String("_") + uid.toString();
}


void ConnectableNode::onContainerParameterChanged(Parameter * p)
{
  nodeListeners.call(&ConnectableNodeListener::nodeParameterChanged, this, p);
}

ConnectableNodeUI * ConnectableNode::createUI()
{
  DBG("No implementation in child node class !");
  jassert(false);
  return nullptr;
}


void ConnectableNode::remove(bool askBeforeRemove)
{
  if (askBeforeRemove)
  {
    int result = AlertWindow::showOkCancelBox(AlertWindow::AlertIconType::QuestionIcon, "Remove node", "Do you want to remove the node ?");
    if (result == 0) return;
  }

  nodeListeners.call(&ConnectableNode::ConnectableNodeListener::askForRemoveNode, this);
}

void ConnectableNode::clear()
{
  //  ControllableContainer::clear();
  //to override
}

var ConnectableNode::getJSONData()
{
  var data = ControllableContainer::getJSONData();
  data.getDynamicObject()->setProperty("nodeType", NodeFactory::nodeToString(this));
  return data;
}

void ConnectableNode::loadJSONDataInternal(var data)
{
  ControllableContainer::loadJSONDataInternal(data);
}



/////////////////////////////  AUDIO


void ConnectableNode::addToAudioGraph(AudioProcessorGraph* g)
{
  audioNode = g->addNode(getAudioProcessor());
  getAudioProcessor()->setRateAndBufferSizeDetails (g->getSampleRate(), g->getBlockSize());

}

void ConnectableNode::removeFromAudioGraph()
{
  const ScopedLock lk(parentNodeContainer->getAudioGraph()->getCallbackLock());
  parentNodeContainer->getAudioGraph()->removeNode(getAudioNode());
  parentNodeContainer->updateAudioGraph(false);

}



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

AudioProcessor * ConnectableNode::getAudioProcessor() {
  if(audioNode){
    jassert(audioNode->getProcessor() == dynamic_cast<NodeBase * >(this));
    return audioNode->getProcessor();
  }
  return dynamic_cast<NodeBase * >(this);
};

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
