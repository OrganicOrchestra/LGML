/*
  ==============================================================================

    NodeBaseHeaderUI.cpp
    Created: 8 Mar 2016 5:53:52pm
    Author:  bkupe

  ==============================================================================
*/

#include "NodeBaseHeaderUI.h"

NodeBaseHeaderUI::NodeBaseHeaderUI()
  {
	  titleLabel.setColour(Label::ColourIds::textColourId, TEXT_COLOR);
	  titleLabel.setJustificationType(Justification::topLeft);
	  titleLabel.setInterceptsMouseClicks(false, false);
	  addAndMakeVisible(titleLabel);

	  node = nullptr;
	  nodeUI = nullptr;

	  setSize(20, 30);
  }

  NodeBaseHeaderUI::~NodeBaseHeaderUI()
  {
	  if (node && node->hasAudioOutputs) {
		  node->audioProcessor->removeListener(&vuMeter);
	  }
  }

  void NodeBaseHeaderUI::setNodeAndNodeUI(NodeBase * node, NodeBaseUI * nodeUI)
{
	this->node = node;
	this->nodeUI = nodeUI;

	titleLabel.setText(node->name, NotificationType::dontSendNotification);
	enabledToggle = node->enabledParam->createToggle();
	addAndMakeVisible(enabledToggle);

	if (node != nullptr && node->hasAudioOutputs) {
		node->audioProcessor->addListener(&vuMeter);
		addAndMakeVisible(vuMeter);
	}
}

void NodeBaseHeaderUI::resized()
{
	if (enabledToggle != nullptr)
	{
		Rectangle<int> r = getLocalBounds();
		r.reduce(5, 2);
		r.removeFromLeft(enabledToggle->getWidth());
		titleLabel.setBounds(r);
		enabledToggle->setTopLeftPosition(5, 5);
	}

	if (node && node->hasAudioOutputs) {
		Rectangle<int> vuMeterRect = getLocalBounds().removeFromRight(14).reduced(4);
		vuMeter.setBounds(vuMeterRect);
	}
}
