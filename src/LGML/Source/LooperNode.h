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
#include "AudioHelpers.h"

class Looper;

class LooperNode : public NodeBase
{

public:
	LooperNode(NodeManager * nodeManager, uint32 nodeId);

	Looper * looper;
	NodeBaseUI * createUI() override;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(LooperNode)
};

#endif  // LOOPERNODE_H_INCLUDED
