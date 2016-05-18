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

class NodeContainerUI :
	public InspectableComponent,
	public NodeContainerListener
{
public:
	NodeContainerUI(NodeContainer * nc);
	virtual ~NodeContainerUI();


	void clear();
	NodeContainer * nodeContainer;

	// Inherited via NodeContainerListener
	virtual void nodeAdded(NodeBase *) override;
	virtual void nodeRemoved(NodeBase *) override;
	virtual void connectionAdded(NodeConnection *) override;
	virtual void connectionRemoved(NodeConnection *) override;
};



#endif  // NODECONTAINERUI_H_INCLUDED
