/*
  ==============================================================================

    NodeBaseUI.h
    Created: 3 Mar 2016 11:52:50pm
    Author:  bkupe

  ==============================================================================
*/

#ifndef NODEBASEUI_H_INCLUDED
#define NODEBASEUI_H_INCLUDED

#include "ConnectableNodeUI.h"

#include "NodeBaseContentUI.h"//keep
#include "NodeBaseHeaderUI.h"//keep
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

	WeakReference<NodeBase> node;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (NodeBaseUI)
};


#endif  // NODEBASEUI_H_INCLUDED
