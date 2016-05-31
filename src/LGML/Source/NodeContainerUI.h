/*
  ==============================================================================

    NodeContainerUI.h
    Created: 18 May 2016 7:54:08pm
    Author:  bkupe

  ==============================================================================
*/

#ifndef NODECONTAINERUI_H_INCLUDED
#define NODECONTAINERUI_H_INCLUDED

#include "NodeContainer.h"
#include "InspectableComponent.h"
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
