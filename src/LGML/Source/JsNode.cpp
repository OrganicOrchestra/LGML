/*
  ==============================================================================

    JsNode.cpp
    Created: 28 May 2016 2:00:46pm
    Author:  Martin Hermant

  ==============================================================================
*/

#include "JsNode.h"

#include "JsNodeUI.h"

ConnectableNodeUI * JsNode::createUI()
{

    NodeBaseUI * ui = new NodeBaseUI(this,new JsNodeUI());
    return ui;
    
}