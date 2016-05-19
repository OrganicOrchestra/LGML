/*
  ==============================================================================

    ConnectableNodeUI.h
    Created: 18 May 2016 11:34:23pm
    Author:  bkupe

  ==============================================================================
*/

#ifndef CONNECTABLENODEUI_H_INCLUDED
#define CONNECTABLENODEUI_H_INCLUDED

#include "InspectableComponent.h"
#include "ConnectableNode.h"

class ConnectableNodeUI : public InspectableComponent
{
public:
	ConnectableNodeUI(ConnectableNode * cn);
	virtual ~ConnectableNodeUI();

	ConnectableNode * connectableNode;
};


#endif  // CONNECTABLENODEUI_H_INCLUDED
