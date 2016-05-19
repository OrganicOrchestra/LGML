/*
  ==============================================================================

    ConnectableNodeUI.cpp
    Created: 18 May 2016 11:34:23pm
    Author:  bkupe

  ==============================================================================
*/

#include "ConnectableNodeUI.h"

ConnectableNodeUI::ConnectableNodeUI(ConnectableNode * cn) : 
	InspectableComponent(cn),
	connectableNode(cn)
 {

}

ConnectableNodeUI::~ConnectableNodeUI()
{
}
