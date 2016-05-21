/*
  ==============================================================================

    SpatNode.cpp
    Created: 2 Mar 2016 8:37:48pm
    Author:  bkupe

  ==============================================================================
*/

#include "SpatNode.h"
#include "NodeBaseUI.h"

SpatNode::SpatNode() :
	NodeBase("Spat",NodeType::SpatType)
{
}

ConnectableNodeUI * SpatNode::createUI() {
	return new NodeBaseUI(this);
}
