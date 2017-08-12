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


#ifndef NODECONTAINERUI_H_INCLUDED
#define NODECONTAINERUI_H_INCLUDED

class NodeContainer;
#include "ConnectableNodeUI.h"

class NodeContainerUI :
	public ConnectableNodeUI
{
public:
	NodeContainerUI(NodeContainer * nc);
	virtual ~NodeContainerUI();

	NodeContainer * nodeContainer;

	void clear();



	JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(NodeContainerUI)
};



#endif  // NODECONTAINERUI_H_INCLUDED
