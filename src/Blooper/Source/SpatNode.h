/*
  ==============================================================================

    SpatNode.h
    Created: 2 Mar 2016 8:37:48pm
    Author:  bkupe

  ==============================================================================
*/

#ifndef SPATNODE_H_INCLUDED
#define SPATNODE_H_INCLUDED




#include "NodeBase.h"

class SpatNode : public NodeBase
{
public:
	SpatNode(uint32 nodeId) :NodeBase(nodeId) {}


	JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(SpatNode)
};


#endif  // SPATNODE_H_INCLUDED
