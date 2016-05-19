/*
  ==============================================================================

    ConnectableNode.cpp
    Created: 18 May 2016 11:33:58pm
    Author:  bkupe

  ==============================================================================
*/

#include "ConnectableNode.h"


  ConnectableNode::ConnectableNode(const String & name) :
	  type(NodeType::UNKNOWN_TYPE),
	  ControllableContainer(name)
  {
	  //set Params
	  nameParam = addStringParameter("Name", "Set the name of the node.", name);
	  enabledParam = addBoolParameter("Enabled", "Set whether the node is enabled or disabled", true);
	  xPosition = addFloatParameter("xPosition", "x position on canvas", 0, 0, 99999);
	  yPosition = addFloatParameter("yPosition", "y position on canvas", 0, 0, 99999);

	  xPosition->isControllableExposed = false;
	  yPosition->isControllableExposed = false;
	  xPosition->isPresettable = false;
	  yPosition->isPresettable = false;
	  nameParam->isPresettable = false;
	  enabledParam->isPresettable = false;
  }

  ConnectableNode::~ConnectableNode()
{
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


void ConnectableNode::parameterValueChanged(Parameter * p)
{
	if (p == nameParam)
	{
		setNiceName(nameParam->stringValue());
	}
	else if (p == enabledParam)
	{
		nodeListeners.call(&ConnectableNodeListener::nodeEnableChanged, this);
	}
	else {
		ControllableContainer::parameterValueChanged(p);
	}
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

var ConnectableNode::getJSONData()
{
	var data = ControllableContainer::getJSONData();
	return data;
}

void ConnectableNode::loadJSONDataInternal(var data)
{
	ControllableContainer::loadJSONDataInternal(data);
}
