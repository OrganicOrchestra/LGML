/*
  ==============================================================================

    VSTNode.h
    Created: 2 Mar 2016 8:37:24pm
    Author:  bkupe

  ==============================================================================
*/

#ifndef VSTNODE_H_INCLUDED
#define VSTNODE_H_INCLUDED


#include "NodeBase.h"

class VSTNode : public NodeBase
{

public:
	VSTNode(uint32 nodeId) :NodeBase(nodeId) {}

	JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(VSTNode)
};



#endif  // VSTNODE_H_INCLUDED
