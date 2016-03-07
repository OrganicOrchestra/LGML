/*
  ==============================================================================

    LooperNode.h
    Created: 3 Mar 2016 10:32:16pm
    Author:  bkupe

  ==============================================================================
*/

#ifndef LOOPERNODE_H_INCLUDED
#define LOOPERNODE_H_INCLUDED




#include "NodeBase.h"

class LooperNode : public NodeBase
{

public:
	LooperNode(NodeManager * nodeManager,uint32 nodeId) :NodeBase(nodeManager,nodeId) {}

	JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(LooperNode)
};

#endif  // LOOPERNODE_H_INCLUDED
