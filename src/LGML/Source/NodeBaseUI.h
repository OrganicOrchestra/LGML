/*
  ==============================================================================

    NodeBaseUI.h
    Created: 3 Mar 2016 11:52:50pm
    Author:  bkupe

  ==============================================================================
*/

#ifndef NODEBASEUI_H_INCLUDED
#define NODEBASEUI_H_INCLUDED

#include "Style.h"
#include "ConnectableNodeUI.h"

#include "NodeBaseContentUI.h"
#include "NodeBaseHeaderUI.h"

class NodeBase;

//==============================================================================
/*

NodeBaseUI provide UI for blocks seen in NodeManagerUI
*/
class NodeBaseUI    :
	public ConnectableNodeUI
{
public:
    NodeBaseUI(NodeBase * node, NodeBaseContentUI * contentContainer = nullptr, NodeBaseHeaderUI * headerContainer = nullptr);
    virtual ~NodeBaseUI();

	NodeBase * node;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (NodeBaseUI)
};


#endif  // NODEBASEUI_H_INCLUDED
