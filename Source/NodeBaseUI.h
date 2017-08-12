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


#ifndef NODEBASEUI_H_INCLUDED
#define NODEBASEUI_H_INCLUDED

#include "ConnectableNodeUI.h"

#include "NodeBaseContentUI.h"//keep
#include "NodeBaseHeaderUI.h"//keep
#include "ParameterUIFactory.h"
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
