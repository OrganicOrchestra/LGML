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


#ifndef NODECONTAINERCONTENTUI_H_INCLUDED
#define NODECONTAINERCONTENTUI_H_INCLUDED

#include "../../UI/ConnectableNodeContentUI.h"
#include "../NodeContainer.h"
#include "../../../Controllable/Parameter/UI/ParameterUI.h"

class NodeContainerContentUI :
	public ConnectableNodeContentUI,
	public ButtonListener,
	public NodeContainerListener
{
public :
	NodeContainerContentUI();
	virtual ~NodeContainerContentUI();

	NodeContainer * nodeContainer;
	TextButton editContainerBT;
	TextButton addUserParamBT;

	OwnedArray<ParameterUI> userParamUI;



	void updateSize();

	void init()override;
	void resized() override;

	void buttonClicked(Button * b) override;

  
	JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(NodeContainerContentUI)
};


#endif  // NODECONTAINERCONTENTUI_H_INCLUDED
